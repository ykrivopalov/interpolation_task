#include "interpolation.h"

#include <algorithm>
#include <iostream>
#include <sstream>

void ThrowInvalidPointError(double x)
{
  std::ostringstream buf;
  buf << "point in not defined interval " << x;
  throw std::runtime_error(buf.str());
}

void ThrowInvalidPointError(double x, double y)
{
  std::ostringstream buf;
  buf << "point in not defined interval (" << x << ", " << y << ")" << std::endl;
  throw std::runtime_error(buf.str());
}

void ThrowGridIsInvalid()
{
  throw std::runtime_error("grid is invalid");
}

Interpolator1D::Interpolator1D(const Array&& data)
  : Data(data)
{
}

std::function<double (Point)> Interpolator1D::GetFunction() const
{
  return std::bind(&Interpolator1D::Evaluate, this, std::placeholders::_1);
}

Array Interpolator1D::Interpolate(const Point& from, const Point& to, double step) const
{
  Array result;
  result.reserve(static_cast<int>((to - from) / step) + 1);
  for (Point x = from; x <= to; x += step)
  {
    const double f = Evaluate(x);
    result.push_back(Pair(x, f));
  }

  return result;
}

double Interpolator1D::Evaluate(double x) const
{
  Array::const_iterator rightBound = std::lower_bound(
    Data.begin(), Data.end(), x,
    [](const Pair& a, const Point& b){ return std::get<0>(a) < b; }
  );

  if (rightBound == Data.end())
    ThrowInvalidPointError(x);

  if (std::get<0>(*rightBound) == x)
    return std::get<1>(*rightBound);

  if (rightBound == Data.begin())
    ThrowInvalidPointError(x);

  Array::const_iterator leftBound = rightBound - 1;

  const double x0 = std::get<0>(*leftBound);
  const double x1 = std::get<0>(*rightBound);
  const double f0 = std::get<1>(*leftBound);
  const double f1 = std::get<1>(*rightBound);
  const double xd = (x - x0) / (x1 - x0);
  return f0 * (1 - xd) + f1 * xd;
}

Interpolator2D::Interpolator2D(const Array2D&& data)
  : Data(data)
{
}

std::function<double (Point2D)> Interpolator2D::GetFunction() const
{
  throw std::string("not impelemented");
}

Array2D Interpolator2D::Interpolate(const Point2D& from, const Point2D& to, double step) const
{
  Array2D result;
  for (double x = std::get<0>(from); x <= std::get<0>(to); x += step)
  {
    Array2D::const_iterator x0RowIter;
    Array2D::const_iterator x1RowIter = std::lower_bound(
      Data.begin(), Data.end(), x,
      [](const DataRow2D& a, const Point& b){ return std::get<0>(a) < b; }
    );

    if (x1RowIter == Data.end())
    {
      ThrowInvalidPointError(x, 0);
    }
    else if (x1RowIter == Data.begin() && std::get<0>(*x1RowIter) == x)
    {
      x0RowIter = x1RowIter;
      x1RowIter = x0RowIter + 1;
    }
    else
    {
      x0RowIter = x1RowIter - 1;
    }

    result.push_back(DataRow2D(x, Array()));
    Array& resultRow = std::get<1>(result.back());
    const Array& x1Row = std::get<1>(*x1RowIter);
    const Array& x0Row = std::get<1>(*x0RowIter);

    const double x0 = std::get<0>(*x0RowIter);
    const double x1 = std::get<0>(*x1RowIter);
    const double xd = (x - x0) / (x1 - x0);

    for (double y = std::get<1>(from); y <= std::get<1>(to); y += step)
    {
      Array::const_iterator x10Data;
      Array::const_iterator x11Data = std::lower_bound(
        x1Row.begin(), x1Row.end(), y,
        [](const Pair& a, const Point& b){ return std::get<0>(a) < b; }
      );

      if (x11Data == x1Row.end())
      {
        ThrowInvalidPointError(y, x);
      }
      else if (x11Data == x1Row.begin() && std::get<0>(*x11Data) == y)
      {
        x10Data = x11Data;
        x11Data = x10Data + 1;
      }
      else
      {
        x10Data = x11Data - 1;
      }

      std::size_t x11Pos = std::distance(x1Row.begin(), x11Data);
      if (x0Row.empty() || x0Row.size() - 1 < x11Pos)
        ThrowGridIsInvalid();

      Array::const_iterator x01Data = x0Row.begin() + x11Pos;
      Array::const_iterator x00Data = x01Data - 1;

      const double y0 = std::get<0>(*x10Data);
      const double y1 = std::get<0>(*x11Data);
      const double f00 = std::get<1>(*x00Data);
      const double f01 = std::get<1>(*x01Data);
      const double f10 = std::get<1>(*x10Data);
      const double f11 = std::get<1>(*x11Data);

      const double yd = (y - y0) / (y1 - y0);
      const double c0 = f00 * (1 - xd) + f10 * xd;
      const double c1 = f01 * (1 - xd) + f11 * xd;
      const double f = c0 * (1 - yd) + c1 * yd;
      resultRow.push_back(Pair(y, f));
    }
  }

  return result;
}

Interpolation::Interpolation()
{
}

std::unique_ptr<Interpolator1D> Interpolation::Interpolate(const Array&& data)
{
  return std::unique_ptr<Interpolator1D>(new Interpolator1D(std::move(data)));
}

std::unique_ptr<Interpolator2D> Interpolation::Interpolate(const Array2D&& data)
{
  return std::unique_ptr<Interpolator2D>(new Interpolator2D(std::move(data)));
}
