/* IntelliType X - Professional Offline Search Intelligence Engine
 * Portable C11 DSA implementation.
 * Build: gcc -std=c11 -O2 -Wall -Wextra intellitype_x.c -o IntelliTypeX.exe
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#define ALPHABET 27
#define MAX_WORDS 500
#define MAX_LEN 100
#define TOP_K 5
#define HISTORY_SIZE 12
#define DATA_FILE "intellitype_profile.dat"
typedef enum { ALL=0,AIML,DSA,DBMS,OS,NETWORKS,PYTHON } Category;
static const char *CAT_NAMES[]={"All","AIML","DSA","DBMS","OS","Networks","Python"};
typedef struct Word Word;
typedef struct TrieNode { struct TrieNode *child[ALPHABET]; Word *terminal; } TrieNode;
struct Word { char text[MAX_LEN]; Category category; int base_frequency; int selections; unsigned long last_selected; };
typedef struct { Word *word; double score; int typo_distance; } Suggestion;
static TrieNode *root; static Word dictionary[MAX_WORDS]; static int word_count;
static unsigned long logical_clock=1; static char history[HISTORY_SIZE][MAX_LEN];
static int history_count,total_searches,total_selections;
static int char_index(char c){if(c>='a'&&c<='z')return c-'a';if(c==' ')return 26;return -1;}
static void normalize(char*s){size_t j=0;for(size_t i=0;s[i]&&j<MAX_LEN-1;i++){unsigned char c=(unsigned char)s[i];if(isalpha(c))s[j++]=(char)tolower(c);else if(isspace(c)&&j>0&&s[j-1]!=' ')s[j++]=' ';}if(j&&s[j-1]==' ')--j;s[j]='\0';}
static TrieNode *new_node(void){return (TrieNode*)calloc(1,sizeof(TrieNode));}
static void trie_insert(const char*text,Word*w){TrieNode*n=root;for(size_t i=0;text[i];i++){int k=char_index(text[i]);if(k<0)continue;if(!n->child[k])n->child[k]=new_node();n=n->child[k];}n->terminal=w;}
static TrieNode *trie_find_prefix(const char*p,int*visited){TrieNode*n=root;if(visited)*visited=1;for(size_t i=0;p[i];i++){int k=char_index(p[i]);if(k<0||!n->child[k])return NULL;n=n->child[k];if(visited)++*visited;}return n;}
static Word *add_word(const char*raw,Category cat,int freq){char s[MAX_LEN];snprintf(s,sizeof s,"%s",raw);normalize(s);if(!s[0]||word_count>=MAX_WORDS)return NULL;for(int i=0;i<word_count;i++)if(!strcmp(dictionary[i].text,s))return &dictionary[i];Word*w=&dictionary[word_count++];memset(w,0,sizeof*w);snprintf(w->text,sizeof w->text,"%s",s);w->category=cat;w->base_frequency=freq;trie_insert(w->text,w);return w;}
static void seed_dictionary(void){
#define ADD(w,c,f) add_word(w,c,f)
ADD("artificial intelligence",AIML,95);ADD("machine learning",AIML,92);ADD("deep learning",AIML,88);ADD("neural network",AIML,84);ADD("natural language processing",AIML,80);ADD("computer vision",AIML,74);ADD("supervised learning",AIML,70);ADD("reinforcement learning",AIML,68);ADD("decision tree",AIML,65);ADD("random forest",AIML,61);ADD("gradient descent",AIML,60);ADD("classification model",AIML,58);
ADD("data structures",DSA,98);ADD("binary search tree",DSA,94);ADD("trie data structure",DSA,90);ADD("graph traversal",DSA,86);ADD("depth first search",DSA,82);ADD("breadth first search",DSA,80);ADD("dijkstra algorithm",DSA,75);ADD("dynamic programming",DSA,72);ADD("hash table",DSA,70);ADD("sorting algorithm",DSA,67);ADD("linked list",DSA,65);ADD("stack operations",DSA,62);ADD("queue implementation",DSA,60);
ADD("database management system",DBMS,96);ADD("structured query language",DBMS,92);ADD("normalization",DBMS,85);ADD("database trigger",DBMS,80);ADD("primary key",DBMS,77);ADD("foreign key",DBMS,74);ADD("b plus tree",DBMS,70);ADD("database indexing",DBMS,68);ADD("transaction management",DBMS,65);
ADD("operating system",OS,97);ADD("process scheduling",OS,91);ADD("deadlock prevention",OS,87);ADD("bankers algorithm",OS,83);ADD("page replacement",OS,80);ADD("virtual memory",OS,77);ADD("file allocation",OS,72);ADD("semaphore",OS,69);ADD("process synchronization",OS,66);
ADD("computer networks",NETWORKS,93);ADD("transmission control protocol",NETWORKS,87);ADD("internet protocol",NETWORKS,84);ADD("domain name system",NETWORKS,79);ADD("routing algorithm",NETWORKS,74);ADD("network topology",NETWORKS,70);ADD("congestion control",NETWORKS,68);
ADD("python programming",PYTHON,95);ADD("list comprehension",PYTHON,88);ADD("dictionary in python",PYTHON,84);ADD("object oriented programming",PYTHON,82);ADD("recursion",PYTHON,78);ADD("exception handling",PYTHON,74);ADD("file handling",PYTHON,70);
#undef ADD
}
static void save_profile(void){FILE*f=fopen(DATA_FILE,"w");if(!f)return;fprintf(f,"%d %d %lu\n",total_searches,total_selections,logical_clock);for(int i=0;i<word_count;i++)fprintf(f,"%d %d %lu\n",i,dictionary[i].selections,dictionary[i].last_selected);fclose(f);}
static void load_profile(void){FILE*f=fopen(DATA_FILE,"r");if(!f)return;if(fscanf(f,"%d %d %lu",&total_searches,&total_selections,&logical_clock)!=3){fclose(f);return;}int i,s;unsigned long t;while(fscanf(f,"%d %d %lu",&i,&s,&t)==3)if(i>=0&&i<word_count){dictionary[i].selections=s;dictionary[i].last_selected=t;}fclose(f);}
static double score_word(const Word*w,Category context){double freq=(double)(w->base_frequency+(w->selections>20?20:w->selections))/120.0;if(freq>1.0)freq=1.0;double recent=w->last_selected?1.0/(1.0+(double)(logical_clock-w->last_selected)):0.0;double ctx=(context==ALL||w->category==context)?1.0:0.0;return 0.55*freq+0.25*recent+0.20*ctx;}
static void top_insert(Suggestion top[TOP_K],int*count,Word*w,Category ctx,int dist){Suggestion x={w,score_word(w,ctx)-0.22*dist,dist};if(*count==TOP_K&&x.score<=top[TOP_K-1].score)return;int pos=*count<TOP_K?*count:TOP_K-1;while(pos>0&&top[pos-1].score<x.score){if(pos<TOP_K)top[pos]=top[pos-1];--pos;}top[pos]=x;if(*count<TOP_K)++*count;}
static void collect_dfs(TrieNode*n,Suggestion top[TOP_K],int*count,Category ctx,int*visited){if(!n)return;if(visited)++*visited;if(n->terminal)top_insert(top,count,n->terminal,ctx,0);for(int i=0;i<ALPHABET;i++)if(n->child[i])collect_dfs(n->child[i],top,count,ctx,visited);}
static int edit_distance(const char*a,const char*b){int m=(int)strlen(a),n=(int)strlen(b),dp[MAX_LEN][MAX_LEN];for(int i=0;i<=m;i++)dp[i][0]=i;for(int j=0;j<=n;j++)dp[0][j]=j;for(int i=1;i<=m;i++)for(int j=1;j<=n;j++){int c=a[i-1]==b[j-1]?0:1;int x=dp[i-1][j]+1,y=dp[i][j-1]+1,z=dp[i-1][j-1]+c;dp[i][j]=x<y?(x<z?x:z):(y<z?y:z);}return dp[m][n];}
static int typo_prefix_distance(const char*q,const char*w){int ql=(int)strlen(q),wl=(int)strlen(w),best=99;if(!ql||!wl)return best;int lo=ql-1,hi=ql+1;if(lo<1)lo=1;if(hi>wl)hi=wl;for(int len=lo;len<=hi;len++){char p[MAX_LEN];memcpy(p,w,(size_t)len);p[len]='\0';int d=edit_distance(q,p);if(d<best)best=d;}return best;}
static int get_suggestions(const char*q,Category ctx,Suggestion top[TOP_K],int*nodes,int*typo){int count=0;if(nodes)*nodes=0;if(typo)*typo=0;TrieNode*p=trie_find_prefix(q,nodes);if(p)collect_dfs(p,top,&count,ctx,nodes);if(!count&&strlen(q)>=3){if(typo)*typo=1;for(int i=0;i<word_count;i++){int d=typo_prefix_distance(q,dictionary[i].text);if(d<=1)top_insert(top,&count,&dictionary[i],ctx,d);}}return count;}
int main(void){root=new_node();if(!root){fprintf(stderr,"Trie allocation failed.\n");return 1;}seed_dictionary();load_profile();char q[MAX_LEN];printf("INTELLITYPE X - Offline C / DSA Search Engine\n");printf("Dictionary terms: %d\n",word_count);printf("Enter a prefix (or quit): ",word_count);while(fgets(q,sizeof q,stdin)){q[strcspn(q,"\r\n")]='\0';normalize(q);if(!strcmp(q,"quit"))break;if(!q[0]){printf("Enter a non-empty prefix: ");continue;}Suggestion top[TOP_K]={0};int nodes=0,typo=0;int count=get_suggestions(q,ALL,top,&nodes,&typo);printf("\n%s results for '%s' (visited nodes: %d)\n",typo?"Fuzzy":"Prefix",q,nodes);for(int i=0;i<count;i++)printf("%d. %-34s score %.3f\n",i+1,top[i].word->text,top[i].score);if(!count)puts("No matches.");printf("\nEnter next prefix (or quit): ");}save_profile();free(root);return 0;}
