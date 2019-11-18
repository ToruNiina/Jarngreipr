#ifndef JARNGREIPR_XYZ_READER_HPP
#define JARNGREIPR_XYZ_READER_HPP
#include <jarngreipr/xyz/XYZParticle.hpp>
#include <jarngreipr/xyz/XYZFrame.hpp>
#include <jarngreipr/util/read_number.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace jarngreipr
{

// lazy XYZ file reader
template<typename realT>
class XYZReader
{
  public:
    using real_type     = realT;
    using particle_type = XYZParticle<realT>;
    using frame_type    = XYZFrame<realT>;

  public:
    explicit XYZReader(const std::string& fname)
        : line_num_(0), filename_(fname), ifstrm_(fname)
    {
        if(!ifstrm_.good())
        {
            log(log_level::error, "XYZReader: file open error: ", fname, '\n');
            std::terminate();
        }
    }
    ~XYZReader() = default;

    bool is_eof() {this->ifstrm_.peek(); return this->ifstrm_.eof();}
    void rewind() {this->ifstrm_.seekg(0, std::ios::beg);}

    frame_type const& read_frame(const std::size_t idx)
    {
        if(this->frames_.size() > idx)
        {
            return this->frames_.at(idx);
        }

        while(!this->is_eof())
        {
            this->frames_.push_back(this->read_next_frame());

            if(this->frames_.size() > idx)
            {
                return this->frames_.at(idx);
            }
        }

        log(log_level::error, "XYZReader: ", this->filename_,
                              " does not contain frame ", idx, ".\n");
        std::terminate();
    }

  private:

    frame_type read_next_frame()
    {
        std::string line;
        std::getline(ifstrm_, line);
        this->line_num_++;

        const auto msg = std::string("while reading XYZ file ")+this->filename_;
        const std::size_t n = read_number<std::size_t>(
                line, 0, line.size(), msg, at_line(this->line_num_));

        frame_type frame;

        std::getline(ifstrm_, frame.comment);
        this->line_num_++;

        for(std::size_t i=0; i<n; ++i)
        {
            std::getline(ifstrm_, line);
            this->line_num_++;
            const auto ln  = at_line(this->line_num_);

            //XXX: this code does redundant stuff to show a better error message

            std::istringstream iss(line);
            std::string ident, crd_x, crd_y, crd_z;
            iss >> ident;
            iss >> crd_x;
            iss >> crd_y;
            iss >> crd_z;
            source_location src(this->filename_, line, 0, line.size(), this->line_num_);
            if(crd_z.empty() || crd_y.empty() || crd_x.empty() || ident.empty())
            {
                log(log_level::error, "XYZReader: line too short.\n", src);
                std::terminate();
            }


            particle_type xyz;
            xyz.name        = ident;
            xyz.position[0] = read_number<real_type>(src, line.find(crd_x), crd_x.size());
            xyz.position[1] = read_number<real_type>(src, line.find(crd_y), crd_y.size());
            xyz.position[2] = read_number<real_type>(src, line.find(crd_z), crd_z.size());

            // XXX: If crd_x and crd_y were the same, line.find(crd_y) returns
            //      the position of crd_x. But practically it does not matter.
            //      Since the string are the same, the value read from them are
            //      also the same. In the case of error, crd_x first writes the
            //      error before crd_y writes the same error.

            frame.particles.push_back(xyz);
        }
        return frame;
    }

  private:
    std::size_t line_num_;
    std::string filename_;
    std::ifstream ifstrm_;
    std::vector<frame_type> frames_;
};

} // jarngreipr
#endif //JARNGREIPR_IO_XYZ_READER
