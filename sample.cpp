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

class CRectangle
{
  // todo
};

class CCirce
{
  // todo
};

class CTriangle
{
  // todo
};

class CPolygon
{
  // todo
};

class CScreen
{
  public:
    void                     add                           ( ... );
    std::vector<int>         test                          ( int               x,
                                                             int               y ) const;
    void                     optimize                      ( void );
    // todo
};



#ifndef __PROGTEST__
int                          main                          ( void )
{
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

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
