#pragma once

#include <tuple>
#include <memory>
#include <vector>
#include <map>

typedef double Point1D;
typedef std::tuple<double, double> Point2D;

typedef std::tuple<Point1D, double> Data1D;
typedef std::tuple<Point2D, double> Data2D;

typedef std::vector<Data1D> DataArray1D;
typedef std::vector<Data2D> DataArray2D;

class Interpolator1D
{
public:
  Interpolator1D(const DataArray1D&& data);

  std::function<double (Point1D)> GetFunction() const;
  DataArray1D Evaluate(const Point1D& from, const Point1D& to, double step) const;

private:
  DataArray1D Data;
};

class Interpolation
{
public:
  Interpolation();

  std::unique_ptr<Interpolator1D> Interpolate(const DataArray1D&& data);
};
