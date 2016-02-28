#include "interpolation.h"

#include <algorithm>
#include <iostream>
#include <sstream>

Interpolator1D::Interpolator1D(const DataArray1D&& data)
  : Data(data)
{
}

std::function<double (Point1D)> Interpolator1D::GetFunction() const
{
  throw std::string("not impelemented");
}

DataArray1D Interpolator1D::Evaluate(const Point1D& from, const Point1D& to, double step) const
{
  std::cout << "Evaluate " << from << " " << to << " " << step << std::endl;
  DataArray1D result;
  result.reserve(static_cast<int>((to - from) / step) + 1);
  for (Point1D i = from; i <= to; i += step)
  {
    DataArray1D::const_iterator rightBound = std::lower_bound(
      Data.begin(), Data.end(), i,
      [](const Data1D& a, const Point1D& b){ return std::get<0>(a) <= b; }
    );

    if (rightBound == Data.end())
    {
      std::ostringstream buf;
      buf << "point in unknown interval left " << i;
      throw buf.str();
    }

    if (std::get<0>(*rightBound) == i)
    {
      result.push_back(Data1D(i, std::get<1>(*rightBound)));
      continue;
    }

    if (rightBound == Data.begin())
    {
      std::ostringstream buf;
      buf << "point in unknown interval right " << i;
      throw buf.str();
    }

    DataArray1D::const_iterator leftBound = rightBound - 1;

    const double coef = (i - std::get<0>(*leftBound)) / (std::get<0>(*rightBound) - std::get<0>(*leftBound));
    const double value = std::get<1>(*leftBound) * (1 - coef) + std::get<1>(*rightBound) * coef;
    result.push_back(Data1D(i, value));
  }

  return result;
}

Interpolation::Interpolation()
{
}

std::unique_ptr<Interpolator1D> Interpolation::Interpolate(const DataArray1D&& data)
{
  return std::unique_ptr<Interpolator1D>(new Interpolator1D(std::move(data)));
}

DataArray1D LoadDataArray1D()
{
  DataArray1D result;
  double x = 0;
  double y = 0;
  while (std::cin >> x >> y)
  {
    result.push_back(Data1D(x, y));
  }

  return result;
}

int main()
{
  DataArray1D data = LoadDataArray1D();

  Interpolation interpolation;
  std::unique_ptr<Interpolator1D> interpolator = interpolation.Interpolate(std::move(data));

  try
  {
    DataArray1D result = interpolator->Evaluate(std::get<0>(data.front()), std::get<0>(data.back()), 0.1);

    for (DataArray1D::const_iterator i = result.begin(); i != result.end(); ++i)
    {
      std::cout << std::get<0>(*i) << " " << std::get<1>(*i) << std::endl;
    }
  }
  catch (const std::string& err)
  {
    std::cout << err << std::endl;
    throw;
  }
}
