#ifndef JARNGREIPR_XYZ_WRITER_HPP
#define JARNGREIPR_XYZ_WRITER_HPP
#include <jarngreipr/xyz/XYZParticle.hpp>
#include <jarngreipr/xyz/XYZFrame.hpp>
#include <stdexcept>
#include <ostream>
#include <fstream>
#include <sstream>

namespace jarngreipr
{

template<typename realT>
class XYZWriter
{
  public:
    typedef XYZParticle<realT> particle_type;
    typedef XYZFrame<realT>    frame_type;

  public:
    explicit XYZWriter(const std::string& fname)
        : filename_(fname), ofstrm_(fname)
    {
        if(!ofstrm_.good())
        {
            log(log_level::error, "XYZWriter: file open error: ", fname, '\n');
            std::exit(EXIT_FAILURE);
        }
    }
    ~XYZWriter() = default;

    void write_frame(const frame_type& frame)
    {
        ofstrm_ << frame.particles.size() << '\n';
        ofstrm_ << frame.comment      << '\n';
        for(const auto& particle : frame.particles)
        {
            ofstrm_ << std::setw(6)  << std::left << particle.name
                    << std::fixed << std::showpoint
                    << std::setprecision(5) << std::right
                    << std::setw(10) << particle.position[0] << ' '
                    << std::setw(10) << particle.position[1] << ' '
                    << std::setw(10) << particle.position[2] << '\n';
        }
        return;
    }

  private:
    std::string filename_;
    std::ofstream ofstrm_;
};

} // jarngreipr
#endif// JARNGREIPR_IO_XYZ_WRITER
