#pragma once

#include <tuple>
#include <memory>
#include <vector>
#include <map>

typedef double Point;
typedef std::tuple<double, double> Pair;
typedef std::vector<Pair> Array;

typedef std::tuple<double, double> Point2D;
typedef std::tuple<double, Array> DataRow2D;
typedef std::vector<DataRow2D> Array2D;

typedef std::tuple<double, double, double> Point3D;
typedef std::tuple<double, Array2D> DataRow3D;
typedef std::vector<DataRow3D> Array3D;

class Interpolator1D
{
public:
  Interpolator1D(const Array&& data);

  std::function<double (Point)> GetFunction() const;
  Array Interpolate(const Point& from, const Point& to, double step) const;

private:
  double Evaluate(double x) const;

  const Array Data;
};

class Interpolator2D
{
public:
  Interpolator2D(const Array2D&& data);

  std::function<double (Point2D)> GetFunction() const;
  Array2D Interpolate(const Point2D& from, const Point2D& to, double step) const;

private:
  const Array2D Data;
};

class Interpolation
{
public:
  Interpolation();

  std::unique_ptr<Interpolator1D> Interpolate(const Array&& data);
  std::unique_ptr<Interpolator2D> Interpolate(const Array2D&& data);
};
