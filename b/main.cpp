#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdint.h>
#include <string.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <deque>
#include <string>

#include <algorithm>
#include <functional>
#include <bitset>
#include <functional>
#include <chrono>
#include <random>

#define sqr(x) (x) * (x)

typedef long long int ll;

using namespace std;
using namespace std::chrono;

//const i64 mod = 1'000'000'000ll + 7;
//const i64 mod = 998'244'353ll;

const ll inf = 1'000'000'000'000'000'000ll;

const long double eps = 1e-8;

struct TRandom {
    using Type = ll;
    mt19937 Rng;
    uniform_int_distribution<Type> Dist;

    TRandom() : Rng((chrono::steady_clock::now().time_since_epoch().count())), Dist(0ll, std::numeric_limits<Type>::max()) {
    }

    Type GetRandomNumber() {
        return Dist(Rng);
    }
};

ll BooksCnt;
ll LibrariesCnt;
ll DaysCnt;

vector<ll> BooksScore;
vector<ll> LibraryBooksCnt;
vector<ll> LibrarySignupDays;
vector<ll> LibraryBooksPerDay;
vector<set<ll>> LibraryBooks;

vector<ll> SignedBooks;

vector<ll> LibrariesSignupOrder;
vector<vector<ll>> LibrarySignupBooks;

void ReadInput(istream& in) {
    in >> BooksCnt >> LibrariesCnt >> DaysCnt;

    BooksScore = vector<ll>(BooksCnt);

    SignedBooks = vector<ll>(BooksCnt);
    LibrarySignupBooks = vector<vector<ll>>(LibrariesCnt);

    for (ll bookId = 0; bookId < BooksCnt; bookId++) {
        in >> BooksScore[bookId];
    }

    LibraryBooksCnt = vector<ll>(LibrariesCnt);
    LibrarySignupDays = vector<ll>(LibrariesCnt);
    LibraryBooksPerDay = vector<ll>(LibrariesCnt);
    LibraryBooks = vector<set<ll>>(LibrariesCnt);

    for (ll libId = 0; libId < LibrariesCnt; libId++) {
        in >> LibraryBooksCnt[libId] >> LibrarySignupDays[libId] >> LibraryBooksPerDay[libId];
        for (ll i = 0; i < LibraryBooksCnt[libId]; i++) {
            ll bookId;
            in >> bookId;
            LibraryBooks[libId].insert(bookId);
        }
    }

    //string t;
    //in >> t;
    //if (!t.empty() || !in.eof()) {
    //    throw 1;
    //}
}

void WriteOutput(ostream& out) {
    out << LibrariesSignupOrder.size() << endl;
    for (ll libId : LibrariesSignupOrder) {
        out << libId << " " << LibrarySignupBooks[libId].size() << endl;
        for (auto bookId : LibrarySignupBooks[libId]) {
            out << bookId << " ";
        }
        out << endl;
    }
}


ll CalculateScore() {
    ll score = 0;
    set<ll> signedBooks;

    ll tm = 0;
    for (ll libId : LibrariesSignupOrder) {
        tm += LibrarySignupDays[libId];

        ll qtm = tm;
        for (ll i = 0; i < LibrarySignupBooks[libId].size(); i++) {
            ll bookId = LibrarySignupBooks[libId][i];
            if (!signedBooks.count(bookId) && qtm < DaysCnt) {
                signedBooks.insert(bookId);
                score += BooksScore[bookId];
            }
            if ((i + 1) % LibraryBooksPerDay[libId] == 0) {
                qtm += 1;
            }
        }
    }

    return score;
}

vector<ll> CalculateLibrariesLimit(vector<ll>& librariesSignupOrder) {
    vector<ll> libsLimit(LibrariesCnt);
    ll tm = 0;
    for (ll libId : librariesSignupOrder) {
        tm += LibrarySignupDays[libId];

        libsLimit[libId] = max(0ll, (DaysCnt - tm) * LibraryBooksPerDay[libId]);
    }

    return libsLimit;
}

void InitLibrariesSignupOrder() {
    set<pair<ll, ll>> libsQueue;
    for (ll libId = 0; libId < LibrariesCnt; libId++) {
        //libsQueue.insert({ LibrarySignupDays[libId], libId });
        libsQueue.insert({ LibrarySignupDays[libId] * 1'000'000ll / LibraryBooks[libId].size(), libId });
    }

    //for (auto [libSignupDays, libId] : libsQueue) {
    //    cerr << libSignupDays << "/" << LibraryBooksPerDay[libId] << "/" << LibraryBooks[libId].size() << endl;
    //}

    for (auto [_, libId] : libsQueue) {
        LibrariesSignupOrder.push_back(libId);
    }
}

void GreedyPickBooks() {
    vector<set<ll>> BookLibs(BooksCnt);
    for (ll libId = 0; libId < LibrariesCnt; libId++) {
        for (auto bookId : LibraryBooks[libId]) {
            BookLibs[bookId].insert(libId);
        }
    }

    //ll bookLibsSum = 0;
    //for (ll bookId = 0; bookId < BooksCnt; bookId++) {
    //    bookLibsSum += BookLibs[bookId].size();
    //}
    //cerr << (double)bookLibsSum / BooksCnt << endl;

    auto libsLimit = CalculateLibrariesLimit(LibrariesSignupOrder);

    //for (ll libId = 0; libId < LibrariesCnt; libId++) {
    //    cerr << libId << ": " << libsLimit[libId] << " " << LibraryBooksPerDay[libId] << " " << LibrarySignupDays[libId] << endl;
    //}


    set<pair<ll, ll>> booksQueue;
    for (ll bookId = 0; bookId < BooksCnt; bookId++) {
        booksQueue.insert({ BooksScore[bookId], bookId });
    }

    ll booksLeftCnt = 0;
    ll booksLeftScore = 0;

    for (auto [_, bookId] : booksQueue) {
        set<pair<ll, ll>, greater<pair<ll, ll>>> availLibs;
        for (auto libId : BookLibs[bookId]) {
            if (!libsLimit[libId]) {
                continue;
            }
            availLibs.insert({ libsLimit[libId], libId });
        }

        if (availLibs.empty()) {
            booksLeftCnt += 1;
            booksLeftScore += BooksScore[bookId];
            continue;
        }

        auto [_, libId] = *availLibs.begin();
        libsLimit[libId] -= 1;
        LibrarySignupBooks[libId].push_back(bookId);
    }

    ll libsLimitLeft = 0;
    for (ll libId = 0; libId < LibrariesCnt; libId++) {
        libsLimitLeft += libsLimit[libId];
    }

    //cerr << libsLimitLeft << "/" << booksLeftCnt << "/" << booksLeftScore << endl;
}

void Solve() {
    TRandom rng;

    InitLibrariesSignupOrder();
    GreedyPickBooks();

    ll prevScore = CalculateScore();
    ll bestScore = prevScore;
    auto bestLibrariesSignupOrder = LibrariesSignupOrder;

    for (ll tt = 0; tt < 100; tt++) {
        ll q = (100 - tt) * 100;

        //ll u = rng.GetRandomNumber() % LibrariesCnt;
        //ll u = 0;
        //ll v = rng.GetRandomNumber() % LibrariesCnt;

        ll u = rng.GetRandomNumber() % LibrariesCnt;
        ll v = (u + (rng.GetRandomNumber() % (100 - tt))) % LibrariesCnt;


        swap(LibrariesSignupOrder[u], LibrariesSignupOrder[v]);

        LibrarySignupBooks = vector<vector<ll>>(LibrariesCnt);
        GreedyPickBooks();
        ll curScore = CalculateScore();

        bool stay = curScore >= prevScore;
        if (!stay) {
            ll m = 1'000'000;
            ll limit = exp(-(double)(prevScore - curScore) / q) * m;
            stay = (rng.GetRandomNumber() % m) < limit;
            if (stay) {
                cerr << prevScore << "->" << curScore << endl;
            }
        }

        if (!stay) {
            swap(u, v);
        }
        else {
            prevScore = curScore;
        }

        if (bestScore < curScore) {
            bestScore = curScore;
            bestLibrariesSignupOrder = LibrariesSignupOrder;
            cerr << tt << ": " << bestScore << endl;
        }

    }
    
    LibrariesSignupOrder = bestLibrariesSignupOrder;
    LibrarySignupBooks = vector<vector<ll>>(LibrariesCnt);
    GreedyPickBooks();

    cerr << "BestScore = " << bestScore << endl;
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(0); cin.tie(0); cout.tie(0); cout.precision(15); cout.setf(ios::fixed); cerr.precision(15); cerr.setf(ios::fixed);

    if (sizeof(ll) != sizeof(long long int)) {
        cerr << "ll != long long int" << endl;
    }

    ReadInput(cin);
    cerr << BooksCnt << "/" << LibrariesCnt << "/" << DaysCnt << endl;

    Solve();

    ll score = CalculateScore();
    cerr << "Score: " << score << endl;

    //WriteOutput(cout);

    return 0;
}