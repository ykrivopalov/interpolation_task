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

  Array3D LoadArray3D()
  {
    Array3D result;
    double x = 0;
    double y = 0;
    double z = 0;
    double f = 0;
    while (std::cin >> x >> y >> z >> f)
    {
      if (!result.size() || std::get<0>(result.back()) != x)
        result.push_back(DataRow3D(x, Array2D()));

      Array2D& yRow = std::get<1>(result.back());
      if (!yRow.size() || std::get<0>(yRow.back()) != y)
        yRow.push_back(DataRow2D(y, Array()));

      std::get<1>(yRow.back()).push_back(Point2D(z, f));
    }

    return result;
  }

  std::string ToString(const DataRow2D& row, const std::string& ident="")
  {
    std::ostringstream buf;
    buf << ident << std::get<0>(row) << std::endl;
    for (Array::const_iterator i = std::get<1>(row).begin(); i != std::get<1>(row).end(); ++i)
    {
      buf << ident << "  " << std::get<0>(*i) << ", f: " << std::get<1>(*i) << std::endl;
    }
    return buf.str();
  }

  std::string ToString(const Array2D& data, const std::string& ident="")
  {
    std::ostringstream buf;
    for (Array2D::const_iterator i = data.begin(); i != data.end(); ++i)
    {
      buf << ToString(*i, ident);
    }
    return buf.str();
  }

  std::string ToString(const DataRow3D& row)
  {
    std::ostringstream buf;
    buf << "" << std::get<0>(row) << std::endl;
    buf << "  " << ToString(std::get<1>(row), "  ") << std::endl;
    return buf.str();
  }

  std::string ToString(const Array3D& data)
  {
    std::ostringstream buf;
    for (Array3D::const_iterator i = data.begin(); i != data.end(); ++i)
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
        Array2D result = interpolator->Interpolate(std::get<1>(data.front()).front(), std::get<1>(data.back()).back(), 0.5);
        std::cout << ToString(result) << std::endl;
      }
      catch (const std::exception& err)
      {
        std::cout << err.what() << std::endl;
        throw;
      }
    }
    else if (argv[1] == std::string("3d"))
    {
      Array3D data = LoadArray3D();
      std::cout << ToString(data) << std::endl;
      Interpolation interpolation;
      std::unique_ptr<Interpolator3D> interpolator = interpolation.Interpolate(std::move(data));
      try
      {
        Array3D result = interpolator->Interpolate(Point3D(1, 1, 1), Point3D(3, 3, 3), 0.5);
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
