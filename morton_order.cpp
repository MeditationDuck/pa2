#ifndef __PROGTEST__
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <memory>
// #include <bitset>
using namespace std;
struct CCoord
{
public:
    CCoord                        ( int               x = 0,
                                    int               y = 0 )
            : m_X ( x ),
              m_Y ( y )
    {
    }
    int                      m_X;
    int                      m_Y;
};
#endif /* __PROGTEST__ */

class Shape {
public:
    int m_id;
    CCoord right_up;
    CCoord left_down;
    Shape(int id):m_id(id){}
    Shape(int id, const CCoord& ru, const CCoord& ld):m_id(id), right_up(ru), left_down(ld){}

    bool contains(int x, int y) const {
        return left_down.m_X <= x && x <= right_up.m_X && left_down.m_Y <= y && y <= right_up.m_Y;
    }

    virtual bool is_at(int x, int y)const = 0;

    virtual Shape* clone() const = 0;

    virtual ~Shape() noexcept = default;
};

class CRectangle: public Shape{
public:
    CRectangle(int id, int x1, int y1, int x2, int y2):Shape(id){
        int max_x = (x1 > x2)? x1:x2;
        int min_x = (x1 < x2)? x1:x2;
        int max_y = (y1 > y2)? y1:y2;
        int min_y = (y1 < y2)? y1:y2;
     
        right_up = CCoord(max_x, max_y);
        left_down = CCoord(min_x, min_y);
    }

    bool is_at([[maybe_unused]] int x, [[maybe_unused]] int y) const override {
        return contains(x, y);
    }

    CRectangle* clone()  const override {return new CRectangle(*this);}
};

class CCircle: public Shape{
    int m_x,m_y;
    long m_r2;
public:
    CCircle(int id, int x, int y, int z):Shape(id,CCoord(x + z, y + z), CCoord(x - z, y - z)),m_x(x),m_y(y),m_r2(static_cast<long>(z)*z){}

    bool is_at(int x, int y) const override {
        return (m_r2 - ( static_cast<long>(m_x - x) * (m_x - x) +  static_cast<long>(m_y - y) * (m_y - y)) >= 0 );
    }
    CCircle* clone()  const override { return new CCircle(*this);}
};

class CTriangle: public Shape{
    CCoord v1;
    CCoord v2;
    CCoord v3;

public:
    CTriangle(int id, const CCoord& a, const CCoord& b, const CCoord& c):Shape(id), v1(a), v2(b), v3(c){
        int max_x = v1.m_X;
        int min_x = v1.m_X;
        int max_y = v1.m_Y;
        int min_y = v1.m_Y;

        if (v2.m_X > max_x) max_x = v2.m_X;
        if (v2.m_X < min_x) min_x = v2.m_X;
        if (v2.m_Y > max_y) max_y = v2.m_Y;
        if (v2.m_Y < min_y) min_y = v2.m_Y;

        if (v3.m_X > max_x) max_x = v3.m_X;
        if (v3.m_X < min_x) min_x = v3.m_X;
        if (v3.m_Y > max_y) max_y = v3.m_Y;
        if (v3.m_Y < min_y) min_y = v3.m_Y;
        left_down = CCoord(min_x, min_y);
        right_up = CCoord(max_x, max_y);
    }

    bool is_at(int x, int y) const override {
        long c1 = static_cast<long>(x - v1.m_X)*(v2.m_Y - v1.m_Y) - static_cast<long>(y - v1.m_Y)*(v2.m_X - v1.m_X);
        long c2 = static_cast<long>(x - v2.m_X)*(v3.m_Y - v2.m_Y) - static_cast<long>(y - v2.m_Y)*(v3.m_X - v2.m_X);
        long c3 = static_cast<long>(x - v3.m_X)*(v1.m_Y - v3.m_Y) - static_cast<long>(y - v3.m_Y)*(v1.m_X - v3.m_X);
        return (c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0);
    }

    CTriangle* clone()  const override {return new CTriangle(*this);}
};

class CPolygon: public Shape{
    vector<CCoord>points;

public:
    CPolygon(int id, vector<CCoord>::const_iterator begin, vector<CCoord>::const_iterator end):Shape(id){
        points.assign(begin, end);
        init();
    }

    CPolygon(int id, const vector<CCoord>& vec):Shape(id){
        points = vec;
        init();
    }

    template<typename... Args>
    CPolygon(int id, Args&&... args):Shape(id){
        points = vector<CCoord>{std::forward<Args>(args)...};
        init();
    }

    void init(){
        auto it = points.begin();
        int max_x = it->m_X;
        int min_x = it->m_X;
        int max_y = it->m_Y;
        int min_y = it->m_Y;
        for(++it; it != points.end() ;++it){
            int curr_x = it->m_X;
            int curr_y = it->m_Y;
            if(curr_x > max_x) max_x = curr_x;
            if(curr_x < min_x) min_x = curr_x;
            if(curr_y > max_y) max_y = curr_y;
            if(curr_y < min_y) min_y = curr_y;
        }
        left_down = CCoord(min_x, min_y);
        right_up = CCoord(max_x, max_y);
    }

    bool is_at(int x, int y) const override {
        size_t size = points.size();
        bool sign = true;
        for (size_t i = 0; i < size; ++i) {
            const CCoord& p1 = points[i];
            const CCoord& p2 = points[(i + 1) % size];
            long c = static_cast<long>(x - p1.m_X) * (p2.m_Y - p1.m_Y) - static_cast<long>(y - p1.m_Y) * (p2.m_X - p1.m_X);
            if (i == 0) {
                sign = (c >= 0);
            } else if ((c >= 0) != sign) return false;
        }
        return true;
    }

    CPolygon* clone()  const override {return new CPolygon(*this);}
};

size_t max_depth = 0;

class Quadtree {
public:
    vector<unique_ptr<Shape>> st_rects;
    vector<Shape*> pt_shapes;
    size_t depth;
    // static const size_t max_tmp = 0;
    static const size_t depth_max = 20;
    array<unique_ptr<Quadtree>, 4>childs;

    Quadtree(size_t dp = 0):depth(dp) {if(depth > max_depth){max_depth = depth;};}
    ~Quadtree(){}
    void make_child(int i){childs[i] = unique_ptr<Quadtree>(new Quadtree(depth+1));}
};

#define max_absolute 1048576

class CScreen{
    Quadtree qt;

    long long mortonNumber(int x, int y) const {
        x += max_absolute-1;
        y += max_absolute-1;
        if(x <0) x = 0;
        if(y < 0) y = 0;

        long long result = 0;
        for (int i = 0; i < 22; ++i) {
            result |= (x & (1ll << i)) << (i + 1);
            result |= (y & (1ll << i)) << i;
        }
        return result;
    }
    
public:
     CScreen(){}
    ~CScreen(){}

    void add(const Shape& shape){
        long long ld_morton = mortonNumber(shape.left_down.m_X, shape.left_down.m_Y);
        long long ru_morton = mortonNumber(shape.right_up.m_X, shape.right_up.m_Y);

        long long xored_shape = ld_morton^ru_morton;

        Quadtree* curr = &qt;

       int index = 0;
        for(;;) {
            int child_index = ((ru_morton>>(40-index*2))&0b11);
            auto next_ru_child = curr->childs[child_index].get();

            if (next_ru_child == nullptr && Quadtree::depth_max >= curr->depth ) {
                curr->make_child(child_index);
            }

            Quadtree* next = nullptr;
            if( next_ru_child != nullptr && ((xored_shape>>(40 - index*2))&0b11) == 0){
                next = next_ru_child;
                index ++;
            }
            if (next != nullptr){
                curr = next;
            } else {
                curr->st_rects.emplace_back(shape.clone());
                return;
            }
        }
    }

    void optimize( void ){

        

    }

    std::vector<int> test(int x, int y) const {
        vector<int> res;
        const Quadtree* curr = &qt;
        int i = 0;
        long long morton = mortonNumber(x, y);
        while(curr){
            for (const auto& e : curr->st_rects) {
                if (e->is_at(x, y)) {
                    res.push_back(e->m_id);
                }
            }
            curr = curr->childs[((morton >> (40 - i * 2)) & 0b11)].get();
            i++;
        }
        return res;
    }

};


#ifndef __PROGTEST__
int rands(){
        return (rand() % (1048576*2)) - 1048576;
    }

#include <chrono>
#include <thread>
using namespace std::chrono;
using namespace std::this_thread;

int main() {

    CScreen s0;
    s0 . add ( CRectangle ( 1, 10, 20, 30, 40 ) );
    s0 . add ( CRectangle ( 2, 20, 10, 40, 30 ) );
    s0 . add ( CTriangle ( 3, CCoord ( 10, 20 ), CCoord ( 20, 10 ), CCoord ( 30, 30 ) ) );
    s0 . optimize();
    assert ( s0 . test ( 0, 0 ) == (vector<int>{ }) );
    assert ( s0 . test ( 21, 21 ) == (vector<int>{ 1, 2, 3 }) );
    assert ( s0 . test ( 16, 17 ) == (vector<int>{ 3 }) );
    assert ( s0 . test ( 30, 22 ) == (vector<int>{ 1, 2 }) );
    assert ( s0 . test ( 35, 25 ) == (vector<int>{ 2 }) );

    CScreen s1;
    s1 . add ( CCircle ( 1, 10, 10, 15 ) );
    s1 . add ( CCircle ( 2, 30, 10, 15 ) );
    s1 . add ( CCircle ( 3, 20, 20, 15 ) );
    s1 . optimize();
    assert ( s1 . test ( 0, 0 ) == (vector<int>{ 1 }) );
    assert ( s1 . test ( 15, 15 ) == (vector<int>{ 1, 3 }) );
    assert ( s1 . test ( 20, 11 ) == (vector<int>{ 1, 2, 3 }) );
    assert ( s1 . test ( 32, 8 ) == (vector<int>{ 2 }) );

    CScreen s2;
    CCoord vertex21[] = { CCoord ( 10, 0 ), CCoord ( 20, 20 ), CCoord ( 30, 20 ), CCoord ( 40, 0 ) };

    s2 . add ( CPolygon ( 1, vertex21, vertex21 + 4 ) );

    CCoord vertex22[] = { CCoord ( 20, 10 ), CCoord ( 10, 20 ), CCoord ( 25, 30 ), CCoord ( 40, 20 ), CCoord ( 30, 10 ) };


    s2 . add ( CPolygon ( 2, vertex22, vertex22 + 5 ) );
    s2 . optimize();
    // vector<int> ret = s2.test(25,15);
    
    assert ( s2 . test ( 25, 15 ) == (vector<int>{ 1, 2 }) );
    assert ( s2 . test ( 25, 25 ) == (vector<int>{ 2 }) );
    assert ( s2 . test ( 15, 3 ) == (vector<int>{ 1 }) );
    assert ( s2 . test ( 11, 10 ) == (vector<int>{ }) );

    CScreen s3;
    std::initializer_list<CCoord> vertex31 = { CCoord ( 10, 0 ), CCoord ( 20, 20 ), CCoord ( 30, 20 ), CCoord ( 40, 0 ) };
    s3 . add ( CPolygon ( 1, vertex31 . begin (), vertex31 . end () ) );
    std::list<CCoord> vertex32 = { CCoord ( 20, 10 ), CCoord ( 10, 20 ), CCoord ( 25, 30 ), CCoord ( 40, 20 ), CCoord ( 30, 10 ) };
    s3 . add ( CPolygon ( 2, vertex32 . begin (), vertex32 . end () ) );
    s3 . optimize();
    assert ( s3 . test ( 25, 15 ) == (vector<int>{ 1, 2 }) );
    assert ( s3 . test ( 25, 25 ) == (vector<int>{ 2 }) );
    assert ( s3 . test ( 15, 3 ) == (vector<int>{ 1 }) );
    assert ( s3 . test ( 11, 10 ) == (vector<int>{ }) );
    s3 . add ( CPolygon ( 3, std::initializer_list<CCoord> { CCoord ( 10, 0 ), CCoord ( 20, 20 ), CCoord ( 30, 20 ), CCoord ( 40, 0 ) } ) );
    s3 . add ( CPolygon ( 4, { CCoord ( 20, 10 ), CCoord ( 10, 20 ), CCoord ( 25, 30 ), CCoord ( 40, 20 ), CCoord ( 30, 10 ) } ) );
    s3 . add ( CPolygon ( 5, CCoord ( 20, 10 ), CCoord ( 10, 20 ), CCoord ( 25, 30 ), CCoord ( 40, 20 ), CCoord ( 30, 10 ) ) );

    // assert ( s3 . test ( 25, 15 ) == (vector<int>{ 1, 2, 3, 4, 5 }) );
    // assert ( s3 . test ( 25, 25 ) == (vector<int>{ 2, 4, 5 }) );
    // assert ( s3 . test ( 15, 3 ) == (vector<int>{ 1, 3 }) );
    // assert ( s3 . test ( 11, 10 ) == (vector<int>{ }) );



    CScreen s4;
    s4.add(CCircle(1, 0 ,0, 4));
    s4.optimize();
    assert(s4.test(4,0) == (vector<int>{1 }));
    assert(s4.test(0,4) == (vector<int>{1 }));
    assert(s4.test(2,0) == (vector<int>{1 }));
    assert(s4.test(2,2) == (vector<int>{1 }));
    assert(s4.test(-4,0) == (vector<int>{1 }));
    assert(s4.test(0,-4) == (vector<int>{1 }));
    assert(s4.test(1,-4) == (vector<int>{ }));

    {        
        srand(time(NULL));
        auto start = high_resolution_clock::now();
        CScreen s5;
        for(int i = 0; i < 100000; ++i){
            switch(rand()%3){
                case 0:
                    s5.add(CRectangle ( i, rands(), rands(), rands(), rands()));
                    break;
                case 1:
                    s5.add ( CCircle ( i, rands(), rands(), rands() ) );
                    break;
                case 2:
                    s5 . add ( CTriangle ( i, CCoord ( rands(), rands() ), CCoord ( rands(), rands() ), CCoord ( rands(), rands() ) ) );
                    break;
            }
            
            
        }
        s5.add(CRectangle(3,-1048576,-1048576,-1048576,-1048576));
        s5.optimize();
        
        vector <int> res;
        for(int i = 0; i < 1000; i++){
            // cout << i << endl;
            res = s5.test(rands(), rands());
            // cout << res.size() << endl;
            // res.clear();
        }  
        auto stop = high_resolution_clock::now();
         auto duration = duration_cast<microseconds>(stop - start);
 
        cout << "Time taken by function: "<< duration.count()/1000000 << "."<< (duration.count()/1000)%1000000 << "," << duration.count()%1000<<" microseconds" << endl; 
        cout << "max depth :"  << max_depth << endl;
        string text;
        getline (cin, text);
        cout <<"comment: " << text << endl << endl;
    }

    CScreen n;

    n . add ( CCircle ( 1, 10, 10, 9 ) );
    n.optimize();

    // CScreen a;

    // a.add(CCircle(1, 2,2, 1));
    // a.add(CCircle(1, 3,3, 1));
    // a.optimize();

    // vector<int> ret = a.test(0,0);

    // for(auto e:ret){
    //     cout << e << " ";
    // } cout << endl;

    // cout << n.test(0,0).size() << endl;
    // cout << n.test(0,0).size() << endl;


    return 0;
}

#endif /* __PROGTEST__ */
