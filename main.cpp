#include "interpolation.h"

#include <iostream>
#include <sstream>

namespace
{
  Array LoadArray()
  {
    Array result;
    double x = 0;
    double y = 0;
    while (std::cin >> x >> y)
    {
      result.push_back(Pair(x, y));
    }

    return result;
  }

  Array2D LoadArray2D()
  {
    Array2D result;
    double x = 0;
    double y = 0;
    double f = 0;
    while (std::cin >> x >> y >> f)
    {
      if (!result.size() || std::get<0>(result.back()) != x)
        result.push_back(DataRow2D(x, Array()));

      std::get<1>(result.back()).push_back(Point2D(y, f));
    }

    return result;
  }

  std::string ToString(const DataRow2D& row)
  {
    std::ostringstream buf;
    buf << "x: " << std::get<0>(row) << std::endl;
    for (Array::const_iterator i = std::get<1>(row).begin(); i != std::get<1>(row).end(); ++i)
    {
      buf << "  y: " << std::get<0>(*i) << ", f: " << std::get<1>(*i) << std::endl;
    }
    return buf.str();
  }

  std::string ToString(const Array2D& data)
  {
    std::ostringstream buf;
    for (Array2D::const_iterator i = data.begin(); i != data.end(); ++i)
    {
      buf << ToString(*i);
    }
    return buf.str();
  }
}

int main(int argc, char** argv)
{
  if (argc > 1)
  {
    if (argv[1] == std::string("1d"))
    {
      Array data = LoadArray();

      Interpolation interpolation;
      std::unique_ptr<Interpolator1D> interpolator = interpolation.Interpolate(std::move(data));

      try
      {
        Array result = interpolator->Interpolate(std::get<0>(data.front()), std::get<0>(data.back()), 0.1);

        for (Array::const_iterator i = result.begin(); i != result.end(); ++i)
        {
          std::cout << std::get<0>(*i) << " " << std::get<1>(*i) << std::endl;
        }
      }
      catch (const std::exception& err)
      {
        std::cout << err.what() << std::endl;
        throw;
      }
    }
    else if (argv[1] == std::string("2d"))
    {
      Array2D data = LoadArray2D();
      std::cout << ToString(data) << std::endl;
      Interpolation interpolation;
      std::unique_ptr<Interpolator2D> interpolator = interpolation.Interpolate(std::move(data));
      try
      {
        Array2D result = interpolator->Interpolate(Point2D(0, 0), Point2D(3, 5), 0.5);
        std::cout << ToString(result) << std::endl;
      }
      catch (const std::exception& err)
      {
        std::cout << err.what() << std::endl;
        throw;
      }
    }
  }
}
