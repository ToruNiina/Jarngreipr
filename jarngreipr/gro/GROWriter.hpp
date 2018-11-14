#ifndef JARNGREIPR_GRO_WRITER_HPP
#define JARNGREIPR_GRO_WRITER_HPP
#include <jarngreipr/gro/GROLine.hpp>
#include <jarngreipr/gro/GROFrame.hpp>
#include <stdexcept>
#include <ostream>
#include <fstream>
#include <sstream>

namespace jarngreipr
{

template<typename realT>
class GROWriter
{
  public:
    typedef GROLine<realT>  line_type;
    typedef GROFrame<realT> frame_type;

  public:
    explicit GROWriter(const std::string& fname)
        : filename_(fname), ofstrm_(fname)
    {
        if(!ofstrm_.good())
        {
            write_error(std::cerr, "GROReader: file open error: ", filename_);
            std::exit(EXIT_FAILURE);
        }
    }
    ~GROWriter() = default;

    void write_frame(const frame_type& frame)
    {
        ofstrm_ << "Jarngreipr output, t= ";
        ofstrm_ << std::right << std::fixed << frame.time << '\n';
        ofstrm_ << std::right << frame.lines.size() << '\n';
        for(const auto& line : frame.lines)
        {
            ofstrm_ << line << '\n';
        }
        ofstrm_ << std::right << std::fixed << frame.box[0] << ' ';
        ofstrm_ << std::right << std::fixed << frame.box[1] << ' ';
        ofstrm_ << std::right << std::fixed << frame.box[2] << '\n';
        return;
    }

  private:
    std::string filename_;
    std::ofstream ofstrm_;
};

} // jarngreipr
#endif// JARNGREIPR_GRO_WRITER_HPP
