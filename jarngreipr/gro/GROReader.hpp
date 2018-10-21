#ifndef JARNGREIPR_GRO_READER_HPP
#define JARNGREIPR_GRO_READER_HPP
#include <jarngreipr/gro/GROLine.hpp>
#include <jarngreipr/gro/GROFrame.hpp>
#include <mjolnir/util/throw_exception.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace jarngreipr
{

// lazy GRO file reader
template<typename realT>
class GROReader
{
  public:
    typedef GROLine<realT>  line_type;
    typedef GROFrame<realT> frame_type;

  public:
    explicit GROReader(const std::string& fname)
        : filename_(fname), ifstrm_(fname)
    {
        if(!ifstrm_.good())
        {
            throw std::runtime_error("jarngreipr::GROeader: file open error: "
                    + filename_);
        }
    }
    ~GROReader() = default;

    frame_type read_next_frame()
    {
        std::string line;
        frame_type frame;

        // Set Time
        std::getline(ifstrm_, line);
        std::size_t pos = line.find("t=");
        if(pos != std::string::npos)
        {
            frame.time = std::stod(line.substr(pos+2);
        } else {
            frame.time = 0.0;
        }

        // Set number of atoms
        std::getline(ifstrm_, line);
        std::size_t n;
        try
        {
            n = std::stoull(line);
        }
        catch(std::invalid_argument const& iva)
        {
            mjolnir::throw_exception<std::runtime_error>(
                "jarngreipr::GROReader: in ", filename_,
                " invalid second line(not a number) appeared: ", line);
        }
        catch(std::out_of_range const& iva)
        {
            mjolnir::throw_exception<std::runtime_error>(
                "jarngreipr::GROReader: in ", filename_,
                " invalid second line(not a number) appeared: ", line);
        }

        // Set atoms
        for(std::size_t i=0; i<n; ++i)
        {
            std::getline(ifstrm_, line);
            std::istringstream iss(line);
            line_type gro_line;
            iss >> gro_line;
            frame.lines.push_back(gro_line);
        }

        // Set box
        std::getline(ifstrm_, line);
        std::string::size_type idx;
        frame.box[0] = std::stod(line, &idx);
        frame.box[1] = std::stod(line.substr(idx), &idx);
        frame.box[2] = std::stod(line.substr(idx));

        return frame;
    }

  private:
    std::string filename_;
    std::ifstream ifstrm_;
};

} // jarngreipr
#endif //JARNGREIPR_GRO_READER_HPP
