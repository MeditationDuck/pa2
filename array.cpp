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
		 CCoord  ( int x = 0, int y = 0 ): m_X ( x ),m_Y ( y ){}
		int                      m_X;
		int                      m_Y;
};
#endif /* __PROGTEST__ */
const double epsilon = 0.0001;

class Shape {
public:
	int m_id;
private:
	CCoord abs_right_up;
	CCoord abs_left_down;
public:
	Shape(int id):m_id(id){}
	Shape(int id, const CCoord& right_up, const CCoord& left_down):m_id(id), abs_right_up(right_up), abs_left_down(left_down){

	}

	void set_abs(const CCoord& right_up, const CCoord& left_down){
		abs_right_up = right_up; abs_left_down = left_down;
	}

	bool abs_is_in(int x, int y) const {
		return abs_right_up.m_X >= x && abs_right_up.m_Y >= y &&
		abs_left_down.m_X <= x && abs_left_down.m_Y <= y;
	}
	
	// virtual bool check_inside() const = 0;

	virtual void opt() = 0;

	virtual bool is_at(int x, int y)const = 0;

	virtual Shape* clone() const = 0;


	virtual ~Shape() noexcept = default;
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

	void opt() override {
		// done at constructor
		// nothing;
	}

	bool is_at([[maybe_unused]] int x, [[maybe_unused]] int y) const override {
		// return Shape::abs_is_in(x, y);
		return true;
	}

	CRectangle* clone()  const override {
        return new CRectangle(*this);
    }

};

class CCircle: public Shape
{
	// todo
	CCoord center;
	int r;
public:
	CCircle(int id, int x, int y, int z):Shape(id,CCoord(x + z, y + z), CCoord(x - z, y - z)),center(CCoord(x, y)),r(z){}

	void opt() override {}

	bool is_at(int x, int y) const override {
		return  static_cast<long>(center.m_X - x) * (center.m_X - x) + static_cast<long>(center.m_Y - y) * (center.m_Y - y) - static_cast<long>(r)*r <= 0;
	}
	CCircle* clone()  const override {
        return new CCircle(*this);
    }

};

// twice of area
long area(int x, int y, const CCoord& p1, const CCoord& p2) {
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

	void opt() override {
	}


	bool is_at(int x, int y) const override {
        long a1 = area(x, y, v2, v3);
        long a2 = area(x, y, v1, v3);
        long a3 = area(x, y, v2, v1);
        return (a1 + a2 + a3) - sum_area < epsilon;
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
        points = vector<CCoord>{forward<Args>(args)...};
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
        double center_x(0) , center_y(0);
        for(size_t i = 0 ; i < size; i++){
            center_x += points[i].m_X;
            center_y += points[i].m_Y;
        }
        center_x/=size;
        center_y/=size;
        sum_area = 0;
        for(size_t i(0) ; i < size; i++){
            sum_area += darea(center_x, center_y, points[i], points[(i+1)%size]);
        }
    }

    double darea(double x, double y, const CCoord& p1, const CCoord& p2) {
        return fabs((x*(p1.m_Y-p2.m_Y) + p1.m_X*(p2.m_Y-y)+ p2.m_X*(y-p1.m_Y)));
    }

	void opt() override {}

    bool is_at(int x, int y) const override {
		size_t size = points.size();
		if(size < 1) return false;
        if(size == 1) return (points[0].m_X == x && points[0].m_Y == y);
		auto& v1 = points[0];
		if(size == 2){
			return (y-v1.m_Y) * static_cast<long>(points[1].m_X - v1.m_X) == static_cast<long>(points[1].m_Y - v1.m_Y) * (x - v1.m_X);
		}

        long point_area(0);
        for(size_t i(0) ; i < size; i++){
            point_area += area(x, y, points[i], points[(i+1)%size]);
        }
//        cout << center_x << ", " << center_y << "  " << x << ", " << y << endl;
//        cout << "point:" << point_area  << " sum:" << sum_area << endl;
//        cout << fabs(point_area - sum_area ) << endl;
        return point_area - sum_area  < epsilon;
	}

	CPolygon* clone()  const override {
        return new CPolygon(*this);
    }
};

class CScreen
{
	vector<unique_ptr<Shape>> shapes;
public:
	void add ( const Shape& shape){
		shapes.push_back(unique_ptr<Shape>(shape.clone()));
	}

	std::vector<int> test (int x, int y) const {
		vector<int> ids;
		for(auto& e: shapes){
			if(e->abs_is_in(x, y) &&  e->is_at(x, y)){
                ids.push_back(e->m_id);
            }
		}
		sort(ids.begin(), ids.end());
		return ids;
	}
	void optimize( void ){
//		for(auto& e:shapes){
//			e->opt();
//		}
		// sort(shapes.begin(),shapes.end(), [&](auto& a, auto& b){
		// 	a->abs_right_up.;
		// })
		//sort by coordinate 

	}

};



#ifndef __PROGTEST__

void show(vector<int> vec){
    cout << "[ ";
	for(auto& e: vec){
		cout << e << " ";
	}
    cout << " ];" << endl;
}

int                          main                          ( void )
{
	CScreen s0;
	s0 . add ( CRectangle ( 1, 10, 20, 30, 40 ) );
	s0 . add ( CRectangle ( 2, 20, 10, 40, 30 ) );
	s0 . add ( CTriangle ( 3, CCoord ( 10, 20 ), CCoord ( 20, 10 ), CCoord ( 30, 30 ) ) );
	s0 . optimize();
    vector<int> v = s0 . test ( 0, 0 );
    for(size_t i = 0; i < v.size(); i++){
        cout << v[i] << " ";
    }cout << endl;
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

    cout << "--------------------" << endl;
	CScreen s2;
	CCoord vertex21[] = { CCoord ( 10, 0 ), CCoord ( 20, 20 ), CCoord ( 30, 20 ), CCoord ( 40, 0 ) };

	s2 . add ( CPolygon ( 1, vertex21, vertex21 + 4 ) );

	CCoord vertex22[] = { CCoord ( 20, 10 ), CCoord ( 10, 20 ), CCoord ( 25, 30 ), CCoord ( 40, 20 ), CCoord ( 30, 10 ) };


	s2 . add ( CPolygon ( 2, vertex22, vertex22 + 5 ) );
	s2 . optimize();
//    show(s2.test(25,15));
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

	return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
