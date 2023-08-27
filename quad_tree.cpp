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



size_t sum_depth = 0;

class Shape {
public:
    int m_id;

    CCoord right_up;
    CCoord left_down;

    Shape(int id):m_id(id){}
    Shape(int id, const CCoord& ru, const CCoord& ld):m_id(id), right_up(ru), left_down(ld){}

    void set_abs(const CCoord& ru, const CCoord& ld){
        right_up = ru; left_down = ld;
    }

    inline bool contains(int x, int y) const {
        return left_down.m_X <= x && x <= right_up.m_X && left_down.m_Y <= y && y <= right_up.m_Y;
    }

    virtual bool is_at(int x, int y)const = 0;

    virtual Shape* clone() const = 0;

    virtual ~Shape() noexcept = default;

    friend ostream& operator << (ostream& os, const Shape& obj){
        os << obj.m_id << " (" << obj.left_down.m_X << ", " << obj.left_down.m_Y << ") (" <<   obj.right_up.m_X << ", " << obj.right_up.m_Y <<")" << endl;
        return os;
    }
};

class CRectangle: public Shape
{
    // todo
public:
    CRectangle(int id, int x1, int y1, int x2, int y2):Shape(id){
        if(x1>x2) swap(x1, x2); 
        if(y1>y2) swap(y1, y2);
        set_abs(CCoord(x2, y2), CCoord(x1, y1));
    }

    bool is_at([[maybe_unused]] int x, [[maybe_unused]] int y) const override {
        // cout << "is at " << endl;
        // sleep_until(system_clock::now() + milliseconds(10));
        
        return contains(x,y);
        // return true;
    }

    CRectangle* clone()  const override {
        return new CRectangle(*this);
    }

};
// float thicken = 0.0001f;

class CCircle: public Shape
{
    int m_x,m_y;
    long m_r2;
public:
    // CCircle(int id, int x, int y, int z):Shape(id,CCoord(x + z, y + z), CCoord(x - z, y - z)),m_x(x),m_y(y),m_r2((int)floorf((z + thicken) * (z + thicken))){}
    CCircle(int id, int x, int y, int z):Shape(id,CCoord(x + z, y + z), CCoord(x - z, y - z)),m_x(x),m_y(y),m_r2(static_cast<long>(z)*z){}

    bool is_at(int x, int y) const override {
        return (m_r2 - ( static_cast<long>(m_x - x) * (m_x - x) +  static_cast<long>(m_y - y) * (m_y - y)) >= 0 );
    }

    CCircle* clone()  const override {
        return new CCircle(*this);
    }

};

inline long area(int x, int y, const CCoord& p1, const CCoord& p2) {
    return fabs((x*(static_cast<long>(p1.m_Y)-p2.m_Y) + p1.m_X*static_cast<long>(p2.m_Y-y)+ p2.m_X*static_cast<long>(y-p1.m_Y)));
}

class CTriangle: public Shape
{
    CCoord v1;
    CCoord v2;
    CCoord v3;
    long sum_area;

public:
    CTriangle(int id, const CCoord& a, const CCoord& b, const CCoord& c):Shape(id), v1(a), v2(b), v3(c){
        // make abstruct rectangle
        int max_x = max({v1.m_X, v2.m_X, v3.m_X});
        int min_x = min({v1.m_X, v2.m_X, v3.m_X});
        int max_y = max({v1.m_Y, v2.m_Y, v3.m_Y});
        int min_y = min({v1.m_Y, v2.m_Y, v3.m_Y});
        set_abs(CCoord(max_x, max_y), CCoord(min_x, min_y));

        //calculate sum_area
        sum_area = area(v1.m_X, v1.m_Y, v2, v3);
    }

    bool is_at(int x, int y) const override {
        long a1 = area(x, y, v2, v3);
        long a2 = area(x, y, v1, v3);
        long a3 = area(x, y, v2, v1);
        return (a1 + a2 + a3) == sum_area;
    }

    CTriangle* clone()  const override {
        return new CTriangle(*this);
    }


};

class CPolygon: public Shape
{
    // todo
    vector<CCoord>points;
    long sum_area;

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
        it++;
        for(; it != points.end() ;it++){
            max_x = max(max_x, it->m_X);
            min_x = min(min_x, it->m_X);
            max_y = max(max_y, it->m_Y);
            min_y = min(min_y, it->m_Y);
        }
        set_abs(CCoord(max_x, max_y), CCoord(min_x, min_y));

        size_t size = points.size();
        sum_area = 0;
        for(size_t i(1) ; i < size; i++){
            sum_area += area(points[0].m_X, points[0].m_Y, points[i], points[(i+1)%size]);
        }
    }

    bool is_at(int x, int y) const override {
        size_t size = points.size();
        if(size < 1) return false;
        if(size == 1) return (points[0].m_X == x && points[0].m_Y == y);
        auto& v1 = points[0];
        if(size == 2){
            return  static_cast<long>(y-v1.m_Y) * (points[1].m_X - v1.m_X) ==  static_cast<long>(points[1].m_Y - v1.m_Y) * (x - v1.m_X);
        }

        long point_area(0);
        for(size_t i(0) ; i < size; i++){
            point_area += area(x, y, points[i], points[(i+1)%size]);
        }

        return point_area == sum_area;
    }

    CPolygon* clone()  const override {
        return new CPolygon(*this);
    }
};


class Rectangle {
public:
    int x1, y1, x2, y2;

    Rectangle(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

    bool contains(int x, int y) const {
        return x1 <= x && x <= x2 && y1 <= y && y <= y2;
    }
    friend ostream& operator << (ostream& os, const Rectangle& r){
        os << "(" << r.x1 << ", " << r.y1 << "), (" << r.x2 << ", " << r.y2 << ")";
        return os;
    }
};

class Quadtree {
public:
    ~Quadtree(){}

    Rectangle bounds;
    vector<unique_ptr<Shape>> st_rects;
    bool divided;
    size_t depth;

    array<unique_ptr<Quadtree>, 4>childs;

    Quadtree(Rectangle bounds,size_t dp = 0)
            : bounds(bounds),divided(false), depth(dp) {
                if(depth>sum_depth) sum_depth = depth;
    }

    bool in_bound(const Shape& shape) const {
        if(bounds.contains(shape.left_down.m_X, shape.left_down.m_Y) && bounds.contains(shape.right_up.m_X, shape.right_up.m_Y) ){
            return true;
        }
        return false;
    }
    void subdivide() {
        int x1 = bounds.x1;
        int y1 = bounds.y1;
        int x2 = bounds.x2;
        int y2 = bounds.y2;
        int halfWidth = (x2 - x1) / 2;
        int halfHeight = (y2 - y1) / 2;

        childs[0] = unique_ptr<Quadtree>(new Quadtree(Rectangle(x1, y1, x1 + halfWidth, y1 + halfHeight), depth+1)); //northwest
        childs[1] = unique_ptr<Quadtree>(new Quadtree(Rectangle(x1 + halfWidth, y1, x2, y1 + halfHeight), depth+1)); //northeast
        childs[2] = unique_ptr<Quadtree>(new Quadtree(Rectangle(x1, y1 + halfHeight, x1 + halfWidth, y2), depth+1)); //southwest
        childs[3] = unique_ptr<Quadtree>(new Quadtree(Rectangle(x1 + halfWidth, y1 + halfHeight, x2, y2), depth+1)); //southeast
        divided = true;
    }
};


class CScreen{
    vector<unique_ptr<Shape>> rect_objs;
    Rectangle bound;
    Quadtree qt;
public:
     CScreen():bound(-1048576,-1048576,1048576,1048576),qt(bound){}
//    CScreen():bound(0,0,0,0){}
    ~CScreen(){}

    void add(const Shape& shape){
        Quadtree* curr = &qt;

        for(;;) {
            if (!curr->divided && curr->depth < 20) {
                curr->subdivide();
            }
            Quadtree* next = nullptr;
            if(curr->divided){
                for (const auto& e : curr->childs) {
                    if (e->in_bound(shape)) {
                        next = e.get();
                        break;
                    }
                }
            }
            if (next != nullptr) {
                curr = next;
            } else {
                curr->st_rects.emplace_back(shape.clone());
                return;
            }
        }
    }

    void optimize( void ){
//        bound = Rectangle(bound.x1, bound.y1, bound.x2, bound.y2);
//    //    cout << "whole size "<< bound << endl;
//        for(auto& e: rect_objs){
//            qt->insert(e);
//        }
//        rect_objs.clear();
    }

    std::vector<int> test(int x, int y) const {
        vector<int> res;
        const Quadtree* curr = &qt;
        for(;;)  {
            for (const auto& e : curr->st_rects) {
                if (e->contains(x, y) && e->is_at(x, y)) {
                    res.push_back(e->m_id);
                }
            }

            if (curr->divided) {
                for (const auto& e : curr->childs) {
                    if (e->bounds.contains(x, y)) {
                        curr = e.get();
                        break;
                    }
                }
            }else{
                break;
            }
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
    s3 . optimize();

    assert ( s3 . test ( 25, 15 ) == (vector<int>{ 1, 2, 3, 4, 5 }) );
    assert ( s3 . test ( 25, 25 ) == (vector<int>{ 2, 4, 5 }) );
    assert ( s3 . test ( 15, 3 ) == (vector<int>{ 1, 3 }) );
    assert ( s3 . test ( 11, 10 ) == (vector<int>{ }) );



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
        CScreen s5;
        for(int i = 0; i < 100000; i++){
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
        auto start = high_resolution_clock::now();
        vector <int> res;
        for(int i = 0; i < 1000; i++){
            // cout << i << endl;
            res = s5.test(rands(), rands());
            // cout << res.size() << endl;
            // res.clear();
        }  
        auto stop = high_resolution_clock::now();
         auto duration = duration_cast<microseconds>(stop - start);
 
        cout << "Time taken by function: "<< duration.count()/1000000 << "."<< (duration.count()/1000)%1000000 << "," << duration.count()%1000<<" microseconds" << endl; cout << "max depth :"  << sum_depth << endl;
        string text;
        getline (cin, text);
        cout <<"comment: " << text << endl << endl;
    }

    CScreen n;

    n . add ( CCircle ( 1, 10, 10, 14 ) );
    n.optimize();

    // cout << n.test(0,0).size() << endl;
    // cout << n.test(0,0).size() << endl;


    return 0;
}

#endif /* __PROGTEST__ */
