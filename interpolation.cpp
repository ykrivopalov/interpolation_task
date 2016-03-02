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


std::runtime_error NotImplementedError()
{
  return std::runtime_error("not implemented");
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
  throw NotImplementedError();
}

template <typename Container, typename Iterator>
void SetBorderIterators(const Container& data, double value, const Iterator& lowerBound, Iterator& x0, Iterator& x1)
{
  if (lowerBound == data.end())
  {
    ThrowInvalidPointError(value);
  }
  else if (lowerBound == data.begin() && std::get<0>(*lowerBound) == value)
  {
    x0 = lowerBound;
    x1 = x0 + 1;
  }
  else
  {
    x1 = lowerBound;
    x0 = x1 - 1;
  }
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

    SetBorderIterators(Data, x, x1RowIter, x0RowIter, x1RowIter);
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

      SetBorderIterators(x1Row, y, x11Data, x10Data, x11Data);
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

Interpolator3D::Interpolator3D(const Array3D&& data)
  : Data(data)
{
}

std::function<double (Point3D)> Interpolator3D::GetFunction() const
{
  throw NotImplementedError();
}

Array3D Interpolator3D::Interpolate(const Point3D& from, const Point3D& to, double step) const
{
  Array3D result;
  for (double x = std::get<0>(from); x <= std::get<0>(to); x += step)
  {
    Array3D::const_iterator x0RowIter;
    Array3D::const_iterator x1RowIter = std::lower_bound(
      Data.begin(), Data.end(), x,
      [](const DataRow3D& a, const Point& b){ return std::get<0>(a) < b; }
    );
    SetBorderIterators(Data, x, x1RowIter, x0RowIter, x1RowIter);
    result.push_back(DataRow3D(x, Array2D()));
    Array2D& resultXRow = std::get<1>(result.back());
    const Array2D& x1Row = std::get<1>(*x1RowIter);
    const Array2D& x0Row = std::get<1>(*x0RowIter);

    const double x0 = std::get<0>(*x0RowIter);
    const double x1 = std::get<0>(*x1RowIter);
    const double xd = (x - x0) / (x1 - x0);

    for (double y = std::get<1>(from); y <= std::get<1>(to); y += step)
    {
      Array2D::const_iterator x10RowIter;
      Array2D::const_iterator x11RowIter = std::lower_bound(
        x1Row.begin(), x1Row.end(), y,
        [](const DataRow2D& a, const Point& b){ return std::get<0>(a) < b; }
      );

      SetBorderIterators(x1Row, y, x11RowIter, x10RowIter, x11RowIter);
      std::size_t x11Pos = std::distance(x1Row.begin(), x11RowIter);
      if (x0Row.empty() || x0Row.size() - 1 < x11Pos)
        ThrowGridIsInvalid();

      Array2D::const_iterator x01RowIter = x0Row.begin() + x11Pos;
      Array2D::const_iterator x00RowIter = x01RowIter - 1;

      resultXRow.push_back(DataRow2D(y, Array()));
      Array& resultYRow = std::get<1>(resultXRow.back());
      const Array& x00Row = std::get<1>(*x00RowIter);
      const Array& x01Row = std::get<1>(*x01RowIter);
      const Array& x10Row = std::get<1>(*x10RowIter);
      const Array& x11Row = std::get<1>(*x11RowIter);

      const double y0 = std::get<0>(*x10RowIter);
      const double y1 = std::get<0>(*x11RowIter);
      const double yd = (y - y0) / (y1 - y0);

      for (double z = std::get<2>(from); y <= std::get<2>(to); y += step)
      {
        Array::const_iterator x000Data;
        Array::const_iterator x001Data = std::lower_bound(
          x00Row.begin(), x00Row.end(), z,
          [](const Pair& a, const Point& b){ return std::get<0>(a) < b; }
        );

        SetBorderIterators(x00Row, z, x001Data, x000Data, x001Data);
        std::size_t x001Pos = std::distance(x00Row.begin(), x001Data);

        if (x01Row.empty() || x01Row.size() - 1 < x001Pos
          || x10Row.empty() || x10Row.size() - 1 < x001Pos
          || x11Row.empty() || x11Row.size() - 1 < x001Pos
        )
          ThrowGridIsInvalid();

        Array::const_iterator x011Data = x01Row.begin() + x001Pos;
        Array::const_iterator x010Data = x011Data - 1;
        Array::const_iterator x101Data = x10Row.begin() + x001Pos;
        Array::const_iterator x100Data = x101Data - 1;
        Array::const_iterator x111Data = x11Row.begin() + x001Pos;
        Array::const_iterator x110Data = x111Data - 1;

        const double z0 = std::get<0>(*x000Data);
        const double z1 = std::get<0>(*x001Data);
        const double zd = (z - z0) / (z1 - z0);

        const double f000 = std::get<1>(*x000Data);
        const double f001 = std::get<1>(*x001Data);
        const double f010 = std::get<1>(*x010Data);
        const double f011 = std::get<1>(*x011Data);
        const double f100 = std::get<1>(*x100Data);
        const double f101 = std::get<1>(*x101Data);
        const double f110 = std::get<1>(*x110Data);
        const double f111 = std::get<1>(*x111Data);

        const double c00 = f000 * (1 - xd) + f100 * xd;
        const double c10 = f010 * (1 - xd) + f110 * xd;
        const double c01 = f001 * (1 - xd) + f101 * xd;
        const double c11 = f011 * (1 - xd) + f111 * xd;

        const double c0 = c00 * (1 - yd) + c10 * yd;
        const double c1 = c01 * (1 - yd) + c11 * yd;

        const double f = c0 * (1 - zd) + c1 * zd;

        resultYRow.push_back(Pair(z, f));
      }
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

std::unique_ptr<Interpolator3D> Interpolation::Interpolate(const Array3D&& data)
{
  return std::unique_ptr<Interpolator3D>(new Interpolator3D(std::move(data)));
}
