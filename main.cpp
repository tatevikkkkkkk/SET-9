/*
 * A1. Анализ строковых сортировок
 * Вывод:   results.csv
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>
using namespace std;

static long long g_char_cmp = 0;

int cmpStr(const string& a, const string& b) {
    int la = (int)a.size(), lb = (int)b.size(), i = 0;
    while (i < la && i < lb) {
        g_char_cmp++;
        if      ((unsigned char)a[i] < (unsigned char)b[i]) return -1;
        else if ((unsigned char)a[i] > (unsigned char)b[i]) return  1;
        i++;
    }
    if (i < la) return 1;
    if (i < lb) return -1;
    return 0;
}

int computeLCP(const string& a, const string& b, int p) {
    int la = (int)a.size(), lb = (int)b.size();
    while (p < la && p < lb && a[p] == b[p]) { g_char_cmp++; p++; }
    if (p < la && p < lb) g_char_cmp++;
    return p;
}

inline int charAt(const string& s, int d) { return d<(int)s.size()?(unsigned char)s[d]:-1; }
inline int cidx (const string& s, int d) { return d<(int)s.size()?(unsigned char)s[d]+1:0; }


class StringGenerator {
public:
    static constexpr const char* ALPHABET =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-.";
    static constexpr int ALPHA_SIZE = 74;
    static constexpr int MIN_LEN    = 10;
    static constexpr int MAX_LEN    = 200;
    static constexpr int MAX_N      = 3000;

    explicit StringGenerator(unsigned seed=42) : rng(seed) {}

    char randChar() { return ALPHABET[rng()%ALPHA_SIZE]; }

    string randString() {
        int len = MIN_LEN + rng()%(MAX_LEN-MIN_LEN+1);
        string s(len,' ');
        for (char& c:s) c=randChar();
        return s;
    }

    string randStringWithPrefix(const string& pfx) {
        int mx = MAX_LEN-(int)pfx.size();
        int mn = max(0,MIN_LEN-(int)pfx.size());
        if (mx<0) return pfx.substr(0,MAX_LEN);
        int extra = mn+rng()%max(1,mx-mn+1);
        string s=pfx; for(int i=0;i<extra;i++) s+=randChar();
        return s;
    }

    vector<string> generateRandom(int n=MAX_N) {
        vector<string> a(n); for(auto& s:a) s=randString(); return a;
    }
    vector<string> generateReverseSorted(int n=MAX_N) {
        auto a=generateRandom(n); sort(a.begin(),a.end(),greater<string>()); return a;
    }
    vector<string> generateAlmostSorted(int n=MAX_N, int k=-1) {
        auto a=generateRandom(n); sort(a.begin(),a.end());
        if(k<0) k=max(1,n/100);
        for(int i=0;i<k;i++) swap(a[rng()%n],a[rng()%n]);
        return a;
    }
    vector<string> generateCommonPrefix(const string& pfx, int n=MAX_N) {
        vector<string> a(n); for(auto& s:a) s=randStringWithPrefix(pfx); return a;
    }
    static vector<string> sub(const vector<string>& a, int n) {
        return {a.begin(), a.begin()+min(n,(int)a.size())};
    }
private:
    mt19937 rng;
};


void stdQuickSort(vector<string>& a, int lo, int hi) {
    if (lo>=hi) return;
    swap(a[lo+(hi-lo)/2], a[hi]);
    int i=lo-1;
    for (int j=lo;j<hi;j++) if(cmpStr(a[j],a[hi])<=0) swap(a[++i],a[j]);
    swap(a[i+1],a[hi]);
    int p=i+1;
    stdQuickSort(a,lo,p-1);
    stdQuickSort(a,p+1,hi);
}

void stdMerge(vector<string>& a, int lo, int mid, int hi) {
    vector<string> L(a.begin()+lo,a.begin()+mid+1);
    vector<string> R(a.begin()+mid+1,a.begin()+hi+1);
    int i=0,j=0,k=lo;
    while(i<(int)L.size()&&j<(int)R.size())
        a[k++] = (cmpStr(L[i],R[j])<=0) ? L[i++] : R[j++];
    while(i<(int)L.size()) a[k++]=L[i++];
    while(j<(int)R.size()) a[k++]=R[j++];
}
void stdMergeSort(vector<string>& a, int lo, int hi) {
    if(lo>=hi) return;
    int mid=lo+(hi-lo)/2;
    stdMergeSort(a,lo,mid); stdMergeSort(a,mid+1,hi); stdMerge(a,lo,mid,hi);
}


void strQuickSort(vector<string>& a, int lo, int hi, int d) {
    if(lo>=hi) return;
    swap(a[lo],a[lo+(hi-lo)/2]);
    int v=charAt(a[lo],d), lt=lo, gt=hi, i=lo+1;
    while(i<=gt) {
        g_char_cmp++;
        int t=charAt(a[i],d);
        if(t<v) swap(a[lt++],a[i++]);
        else if(t>v) swap(a[i],a[gt--]);
        else i++;
    }
    strQuickSort(a,lo,lt-1,d);
    if(v>=0) strQuickSort(a,lt,gt,d+1);
    strQuickSort(a,gt+1,hi,d);
}

vector<int> strMergeSort(vector<string>& a, int lo, int hi);

vector<int> strMergeLCP(vector<string>& a, int lo, int mid, int hi,
                         const vector<int>& lcpL, const vector<int>& lcpR) {
    int nl=mid-lo+1, nr=hi-mid;
    vector<string> L(a.begin()+lo,   a.begin()+mid+1);
    vector<string> R(a.begin()+mid+1,a.begin()+hi+1);
    int i=0,j=0,k=lo, p=0;
    while(i<nl && j<nr) {
        int d = computeLCP(L[i],R[j],p);
        int li=(int)L[i].size(), rj=(int)R[j].size();
        bool lw;
        if(d==li&&d==rj) lw=true;
        else if(d==li)   lw=true;
        else if(d==rj)   lw=false;
        else lw=((unsigned char)L[i][d]<(unsigned char)R[j][d]);
        if(lw) {
            a[k++]=L[i];
            p=(i+1<nl)?min((int)lcpL[i],d):0;
            i++;
        } else {
            a[k++]=R[j];
            p=(j+1<nr)?min((int)lcpR[j],d):0;
            j++;
        }
    }
    while(i<nl) a[k++]=L[i++];
    while(j<nr) a[k++]=R[j++];
    vector<int> res(hi-lo);
    for(int x=0;x<hi-lo;x++) {
        int pos=0;
        while(pos<(int)a[lo+x].size()&&pos<(int)a[lo+x+1].size()&&a[lo+x][pos]==a[lo+x+1][pos]) pos++;
        res[x]=pos;
    }
    return res;
}

vector<int> strMergeSort(vector<string>& a, int lo, int hi) {
    if(lo>=hi) return {};
    int mid=lo+(hi-lo)/2;
    auto lL=strMergeSort(a,lo,mid);
    auto lR=strMergeSort(a,mid+1,hi);
    lL.resize(mid-lo,0);
    lR.resize(hi-mid,0);
    return strMergeLCP(a,lo,mid,hi,lL,lR);
}

const int R_ALPHA=256;
void msdRadix(vector<string>& a, vector<string>& aux, int lo, int hi, int d) {
    if(lo>=hi) return;
    int freq[R_ALPHA+2]={};
    for(int i=lo;i<=hi;i++) freq[cidx(a[i],d)+1]++;
    for(int r=0;r<=R_ALPHA;r++) freq[r+1]+=freq[r];
    int st[R_ALPHA+2]; copy(freq,freq+R_ALPHA+2,st);
    for(int i=lo;i<=hi;i++) aux[lo+freq[cidx(a[i],d)]++]=a[i];
    for(int i=lo;i<=hi;i++) a[i]=aux[i];
    for(int r=1;r<=R_ALPHA;r++) {
        int blo=lo+st[r],bhi=lo+st[r+1]-1;
        if(blo<bhi) msdRadix(a,aux,blo,bhi,d+1);
    }
}

const int CUTOFF_RQ=74;
void msdRadixQuick(vector<string>& a, vector<string>& aux, int lo, int hi, int d) {
    if(lo>=hi) return;
    if(hi-lo+1<CUTOFF_RQ){ strQuickSort(a,lo,hi,d); return; }
    int freq[R_ALPHA+2]={};
    for(int i=lo;i<=hi;i++) freq[cidx(a[i],d)+1]++;
    for(int r=0;r<=R_ALPHA;r++) freq[r+1]+=freq[r];
    int st[R_ALPHA+2]; copy(freq,freq+R_ALPHA+2,st);
    for(int i=lo;i<=hi;i++) aux[lo+freq[cidx(a[i],d)]++]=a[i];
    for(int i=lo;i<=hi;i++) a[i]=aux[i];
    for(int r=1;r<=R_ALPHA;r++) {
        int blo=lo+st[r],bhi=lo+st[r+1]-1;
        if(blo<bhi) msdRadixQuick(a,aux,blo,bhi,d+1);
    }
}

struct BenchResult {
    string algorithm, arrayType;
    int n;
    double timeUs;
    long long charCmps;
};

class StringSortTester {
public:
    static constexpr int RUNS = 5;

    template<typename Fn>
    BenchResult bench(const string& name, const string& type,
                      const vector<string>& input, Fn fn) {
        double totT=0; long long totC=0;
        for(int r=0;r<RUNS;r++){
            vector<string> a=input;
            g_char_cmp=0;
            auto t0=chrono::high_resolution_clock::now();
            fn(a);
            auto t1=chrono::high_resolution_clock::now();
            totT+=chrono::duration<double,micro>(t1-t0).count();
            totC+=g_char_cmp;
        }
        return {name,type,(int)input.size(),totT/RUNS,totC/RUNS};
    }

    vector<BenchResult> runAll(const vector<string>& arr, const string& tp) {
        vector<BenchResult> res;
        res.push_back(bench("StdQuickSort",tp,arr,[](vector<string>&a){if(a.size()>1)stdQuickSort(a,0,(int)a.size()-1);}));
        res.push_back(bench("StdMergeSort",tp,arr,[](vector<string>&a){if(a.size()>1)stdMergeSort(a,0,(int)a.size()-1);}));
        res.push_back(bench("StrQuickSort",tp,arr,[](vector<string>&a){if(a.size()>1)strQuickSort(a,0,(int)a.size()-1,0);}));
        res.push_back(bench("StrMergeSort",tp,arr,[](vector<string>&a){if(a.size()>1)strMergeSort(a,0,(int)a.size()-1);}));
        res.push_back(bench("MsdRadix",tp,arr,[](vector<string>&a){if(a.size()>1){vector<string>aux(a.size());msdRadix(a,aux,0,(int)a.size()-1,0);}}));
        res.push_back(bench("MsdRadixQuick",tp,arr,[](vector<string>&a){if(a.size()>1){vector<string>aux(a.size());msdRadixQuick(a,aux,0,(int)a.size()-1,0);}}));
        return res;
    }

    vector<BenchResult> runFull(unsigned seed=42) {
        StringGenerator gen(seed);
        auto bRand  = gen.generateRandom();
        auto bRev   = gen.generateReverseSorted();
        auto bAlm   = gen.generateAlmostSorted();
        auto bPfx   = gen.generateCommonPrefix("COMMONPREFIX");
        vector<BenchResult> all;
        for(int n=100;n<=StringGenerator::MAX_N;n+=100){
            cerr<<"\r  n="<<setw(4)<<n<<flush;
            for(auto&r:runAll(StringGenerator::sub(bRand, n),"random"))  all.push_back(r);
            for(auto&r:runAll(StringGenerator::sub(bRev,  n),"reverse")) all.push_back(r);
            for(auto&r:runAll(StringGenerator::sub(bAlm,  n),"almost"))  all.push_back(r);
            for(auto&r:runAll(StringGenerator::sub(bPfx,  n),"prefix"))  all.push_back(r);
        }
        cerr<<"\n";
        return all;
    }

    static void saveCSV(const vector<BenchResult>& res, const string& path) {
        ofstream f(path);
        f<<"algorithm,array_type,n,time_us,char_cmps\n";
        for(const auto&r:res)
            f<<r.algorithm<<","<<r.arrayType<<","<<r.n<<","
             <<fixed<<setprecision(2)<<r.timeUs<<","<<r.charCmps<<"\n";
        cerr<<"Saved: "<<path<<"\n";
    }

    static void printSummary(const vector<BenchResult>& res, int N) {
        cout<<"\n── n="<<N<<" ───────────────────────────────────────\n";
        cout<<left<<setw(16)<<"Algorithm"<<setw(10)<<"Type"
            <<setw(13)<<"Time(µs)"<<"CharCmps\n"<<string(56,'-')<<"\n";
        for(const auto&r:res)
            if(r.n==N)
                cout<<left<<setw(16)<<r.algorithm<<setw(10)<<r.arrayType
                    <<setw(13)<<fixed<<setprecision(1)<<r.timeUs<<r.charCmps<<"\n";
    }

    static bool verify(const string& name, vector<string> a) {
        vector<string> exp=a; sort(exp.begin(),exp.end());
        g_char_cmp=0;
        if(name=="StdQuickSort")  stdQuickSort(a,0,(int)a.size()-1);
        else if(name=="StdMergeSort")  stdMergeSort(a,0,(int)a.size()-1);
        else if(name=="StrQuickSort")  strQuickSort(a,0,(int)a.size()-1,0);
        else if(name=="StrMergeSort")  strMergeSort(a,0,(int)a.size()-1);
        else if(name=="MsdRadix")     {vector<string>aux(a.size());msdRadix(a,aux,0,(int)a.size()-1,0);}
        else if(name=="MsdRadixQuick"){vector<string>aux(a.size());msdRadixQuick(a,aux,0,(int)a.size()-1,0);}
        bool ok=(a==exp);
        cout<<"  ["<<(ok?"OK  ":"FAIL")<<"] "<<name<<"\n";
        return ok;
    }
};


int main(){
    ios_base::sync_with_stdio(false);

    cout<<"=== Верификация ===\n";
    {
        StringGenerator gen(999);
        auto arr=gen.generateRandom(500);
        vector<string> algos={"StdQuickSort","StdMergeSort","StrQuickSort",
                               "StrMergeSort","MsdRadix","MsdRadixQuick"};
        bool ok=true;
        cout<<"  -- случайный --\n";
        for(const auto&a:algos) ok&=StringSortTester::verify(a,arr);
        cout<<"  -- отсортированный --\n";
        auto srt=arr; sort(srt.begin(),srt.end());
        for(const auto&a:algos) ok&=StringSortTester::verify(a,srt);
        cout<<"  -- обратный --\n";
        auto rev=srt; reverse(rev.begin(),rev.end());
        for(const auto&a:algos) ok&=StringSortTester::verify(a,rev);
        if(!ok){ cerr<<"ОШИБКА: есть неверные реализации!\n"; return 1; }
        cout<<"Все алгоритмы прошли верификацию.\n";
    }

    cout<<"\n=== Эмпирический анализ (n=100..3000, шаг 100, "
        <<StringSortTester::RUNS<<" запусков на точку) ===\n";
    StringSortTester tester;
    auto results=tester.runFull(42);

    StringSortTester::saveCSV(results,"results.csv");

    StringSortTester::printSummary(results,500);
    StringSortTester::printSummary(results,1000);
    StringSortTester::printSummary(results,3000);

    return 0;
}