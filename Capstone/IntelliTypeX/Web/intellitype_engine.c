/* IntelliType X Web Edition - C11 Trie engine compiled to WebAssembly.
   Prefix retrieval, ranking and typo recovery execute in C. */
typedef unsigned int u32;
typedef unsigned char u8;
#define MAX_NODES 4096
#define MAX_WORDS 256
#define OUT_SIZE 8192
#define ALPHABET 27
typedef struct { int child[ALPHABET]; int terminal; int word_id; } TrieNode;
typedef struct { char word[64]; char category[16]; char description[100]; int frequency; int recency; } Word;
static TrieNode nodes[MAX_NODES];
static Word words[MAX_WORDS];
static int node_count=1, word_count=0, clock_tick=1;
static char output[OUT_SIZE], input_buffer[128];
static int top_ids[8], top_scores[8], top_count=0, initialized=0;
static int idx(char c){if(c>='A'&&c<='Z')c=(char)(c-'A'+'a');if(c>='a'&&c<='z')return c-'a';if(c==' '||c=='-'||c=='_')return 26;return -1;}
static int eq(const char*a,const char*b){int i=0;while(a[i]&&b[i]&&a[i]==b[i])i++;return a[i]==b[i];}
static int len(const char*s){int i=0;while(s[i])i++;return i;}
static void copy(char*d,const char*s,int n){int i=0;for(;s[i]&&i<n-1;i++)d[i]=s[i];d[i]=0;}
static void clear_node(int n){for(int i=0;i<ALPHABET;i++)nodes[n].child[i]=0;nodes[n].terminal=0;nodes[n].word_id=-1;}
static int new_node(void){if(node_count>=MAX_NODES)return 0;int n=node_count++;clear_node(n);return n;}
static void insert_word(const char*w,const char*cat,const char*desc,int freq){
 int id=word_count++;if(id>=MAX_WORDS)return;
 copy(words[id].word,w,63);copy(words[id].category,cat,15);copy(words[id].description,desc,99);
 words[id].frequency=freq;words[id].recency=0;int n=0;
 for(int i=0;w[i];i++){int k=idx(w[i]);if(k<0)continue;if(!nodes[n].child[k])nodes[n].child[k]=new_node();n=nodes[n].child[k];}
 nodes[n].terminal=1;nodes[n].word_id=id;
}
static void init_engine(void){
 if(initialized)return;initialized=1;clear_node(0);
 insert_word("machine learning","AIML","Supervised learning from data",96);
 insert_word("deep learning","AIML","Neural networks with multiple layers",91);
 insert_word("artificial intelligence","AIML","Systems performing intelligent tasks",88);
 insert_word("neural network","AIML","Connected computational neurons",83);
 insert_word("computer vision","AIML","Understanding images and video",75);
 insert_word("natural language processing","AIML","Computational language understanding",79);
 insert_word("reinforcement learning","AIML","Learning through rewards",72);
 insert_word("gradient descent","AIML","Optimization algorithm",68);
 insert_word("regression","AIML","Predictive statistical method",65);
 insert_word("classification","AIML","Predicting discrete labels",62);
 insert_word("decision tree","AIML","Tree-based predictive model",60);
 insert_word("random forest","AIML","Ensemble of decision trees",57);
 insert_word("support vector machine","AIML","Maximum-margin classifier",54);
 insert_word("data preprocessing","AIML","Cleaning and transforming data",52);
 insert_word("backpropagation","AIML","Neural network gradient calculation",50);
 insert_word("trie","DSA","Prefix tree for fast retrieval",95);
 insert_word("binary search tree","DSA","Ordered binary tree",88);
 insert_word("graph traversal","DSA","Visiting graph vertices",81);
 insert_word("depth first search","DSA","Graph traversal using a stack",84);
 insert_word("breadth first search","DSA","Level-order graph traversal",82);
 insert_word("dynamic programming","DSA","Optimization using overlapping subproblems",92);
 insert_word("linked list","DSA","Nodes connected by pointers",75);
 insert_word("hash table","DSA","Key-value lookup structure",78);
 insert_word("quick sort","DSA","Partition-based sorting algorithm",77);
 insert_word("merge sort","DSA","Divide-and-conquer sorting",76);
 insert_word("heap","DSA","Priority-based tree structure",73);
 insert_word("time complexity","DSA","Growth rate of an algorithm",89);
 insert_word("space complexity","DSA","Memory usage of an algorithm",80);
 insert_word("database normalization","DBMS","Organizing relational tables",90);
 insert_word("sql","DBMS","Structured Query Language",96);
 insert_word("primary key","DBMS","Unique row identifier",84);
 insert_word("foreign key","DBMS","Referential integrity constraint",82);
 insert_word("indexing","DBMS","Accelerating database queries",79);
 insert_word("transactions","DBMS","Atomic database operations",77);
 insert_word("acid properties","DBMS","Transaction reliability principles",86);
 insert_word("join operation","DBMS","Combining relational tables",74);
 insert_word("operating system","OS","Manages computer resources",94);
 insert_word("process scheduling","OS","Allocating CPU time",88);
 insert_word("deadlock","OS","Processes waiting on resources",83);
 insert_word("virtual memory","OS","Memory abstraction technique",81);
 insert_word("page replacement","OS","Managing memory pages",79);
 insert_word("bankers algorithm","OS","Deadlock avoidance algorithm",85);
 insert_word("computer networks","Networks","Connected computing devices",90);
 insert_word("tcp ip","Networks","Internet protocol suite",92);
 insert_word("routing algorithm","Networks","Selecting network paths",79);
 insert_word("congestion control","Networks","Managing network traffic",82);
 insert_word("python","Python","High-level programming language",96);
 insert_word("data structures","Python","Organizing information efficiently",80);
 insert_word("list comprehension","Python","Concise list construction",74);
 insert_word("object oriented programming","Python","Programming with objects",87);
 insert_word("recursion","Python","Function calling itself",78);
 insert_word("file handling","Python","Reading and writing files",71);
 insert_word("exception handling","Python","Managing runtime errors",73);
 insert_word("lambda function","Python","Anonymous function expression",69);
 insert_word("dictionary","Python","Key-value collection",84);
}
static int score_word(int id,int cat){
 int s=words[id].frequency*10+words[id].recency*18;
 if(cat==0||eq(words[id].category,cat==1?"AIML":cat==2?"DSA":cat==3?"DBMS":cat==4?"OS":cat==5?"Networks":"Python"))s+=180;
 return s;
}
static void add_top(int id,int score){
 int pos=top_count;if(pos>4)pos=4;
 while(pos>0&&top_scores[pos-1]<score){if(pos<5){top_scores[pos]=top_scores[pos-1];top_ids[pos]=top_ids[pos-1];}pos--;}
 if(pos<5){top_scores[pos]=score;top_ids[pos]=id;if(top_count<5)top_count++;}
}
static int prefix_node(const char*p){int n=0;for(int i=0;p[i];i++){int k=idx(p[i]);if(k<0)return -1;n=nodes[n].child[k];if(!n)return -1;}return n;}
static void collect(int n,int cat){
 if(nodes[n].terminal){int id=nodes[n].word_id;if(id>=0&&(cat==0||eq(words[id].category,cat==1?"AIML":cat==2?"DSA":cat==3?"DBMS":cat==4?"OS":cat==5?"Networks":"Python")))add_top(id,score_word(id,cat));}
 for(int i=0;i<ALPHABET;i++)if(nodes[n].child[i])collect(nodes[n].child[i],cat);
}
static int min3(int a,int b,int c){int m=a<b?a:b;return m<c?m:c;}
static int edit_distance(const char*a,const char*b){
 int x=len(a),y=len(b);if(x>60||y>60)return 99;int prev[64],cur[64];
 for(int j=0;j<=y;j++)prev[j]=j;
 for(int i=1;i<=x;i++){cur[0]=i;for(int j=1;j<=y;j++)cur[j]=min3(prev[j]+1,cur[j-1]+1,prev[j-1]+(a[i-1]==b[j-1]?0:1));for(int j=0;j<=y;j++)prev[j]=cur[j];}
 return prev[y];
}
static int prefix_edit_distance(const char*prefix,const char*word){
 int n=len(prefix);char candidate[64];int i=0;while(i<n&&word[i]){candidate[i]=word[i];i++;}candidate[i]=0;return edit_distance(prefix,candidate);
}
static int append(char*o,int at,const char*s){while(*s&&at<OUT_SIZE-2)o[at++]=*s++;return at;}
static int number(char*o,int at,int v){char t[16];int n=0;if(v==0)t[n++]='0';while(v>0&&n<15){t[n++]=(char)('0'+v%10);v/=10;}while(n>0&&at<OUT_SIZE-2)o[at++]=t[--n];return at;}
static int quoted(char*o,int at,const char*s){at=append(o,at,"\"");for(int i=0;s[i]&&at<OUT_SIZE-8;i++){char c=s[i];if(c=='"'||c=='\\')o[at++]='\\';if(c=='\n')continue;o[at++]=c;}return append(o,at,"\"");}
__attribute__((visibility("default"))) int get_input_ptr(void){return (int)(unsigned long)input_buffer;}
__attribute__((visibility("default"))) int search_json(const char*prefix,int cat){
 init_engine();top_count=0;int p=prefix_node(prefix);int typo=0;if(p>=0)collect(p,cat);
 if(top_count==0&&len(prefix)>=3){int best=99;for(int i=0;i<word_count;i++){if(cat&&!eq(words[i].category,cat==1?"AIML":cat==2?"DSA":cat==3?"DBMS":cat==4?"OS":cat==5?"Networks":"Python"))continue;int d=prefix_edit_distance(prefix,words[i].word);if(d<best)best=d;}
 if(best<=2){typo=1;for(int i=0;i<word_count;i++){if(cat&&!eq(words[i].category,cat==1?"AIML":cat==2?"DSA":cat==3?"DBMS":cat==4?"OS":cat==5?"Networks":"Python"))continue;int d=prefix_edit_distance(prefix,words[i].word);if(d==best)add_top(i,score_word(i,cat)-d*100);}}}
 int at=0;at=append(output,at,"{\"count\":");at=number(output,at,top_count);at=append(output,at,",\"typo\":");at=append(output,at,typo?"true":"false");at=append(output,at,",\"items\":[");
 for(int i=0;i<top_count;i++){int id=top_ids[i];if(i)at=append(output,at,",");at=append(output,at,"{\"word\":");at=quoted(output,at,words[id].word);at=append(output,at,",\"category\":");at=quoted(output,at,words[id].category);at=append(output,at,",\"description\":");at=quoted(output,at,words[id].description);at=append(output,at,",\"frequency\":");at=number(output,at,words[id].frequency);at=append(output,at,",\"score\":");at=number(output,at,top_scores[i]);at=append(output,at,"}");}
 at=append(output,at,"]}");output[at]=0;return (int)(unsigned long)output;
}
__attribute__((visibility("default"))) void select_word(const char*w){init_engine();for(int i=0;i<word_count;i++)if(eq(words[i].word,w)){words[i].frequency+=3;words[i].recency=clock_tick++;break;}}
__attribute__((visibility("default"))) int get_word_count(void){init_engine();return word_count;}
__attribute__((visibility("default"))) int get_node_count(void){init_engine();return node_count;}
__attribute__((visibility("default"))) int get_dictionary_json(void){init_engine();int at=0;at=append(output,at,"[");for(int i=0;i<word_count;i++){if(i)at=append(output,at,",");at=quoted(output,at,words[i].word);}at=append(output,at,"]");output[at]=0;return (int)(unsigned long)output;}
