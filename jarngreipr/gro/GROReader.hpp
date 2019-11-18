#ifndef JARNGREIPR_GRO_READER_HPP
#define JARNGREIPR_GRO_READER_HPP
#include <jarngreipr/gro/GROLine.hpp>
#include <jarngreipr/gro/GROFrame.hpp>
#include <jarngreipr/io/read_number.hpp>
#include <jarngreipr/util/log.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace jarngreipr
{

template<typename realT>
class GROReader
{
  public:
    using real_type  = realT;
    using line_type  = GROLine<realT>;
    using frame_type = GROFrame<realT>;

  public:
    explicit GROReader(const std::string& fname)
        : line_num_(0), filename_(fname), ifstrm_(fname)
    {
        if(!ifstrm_.good())
        {
            log(log_level::error, "GROReader: file open error: ", filename_, '\n');
            std::terminate();
        }
    }
    ~GROReader() = default;

    bool is_eof() {this->ifstrm_.peek(); return this->ifstrm_.eof();}
    void rewind() {this->ifstrm_.seekg(0, std::ios::beg);}

    frame_type read_next_frame()
    {
        std::string line;
        frame_type frame;
        frame.time = 0.0;

        // Set Time
        this->get_line(line);
        const auto pos = line.find("t=");
        if(pos != std::string::npos)
        {
            const auto t = line.substr(pos+2);
            frame.time = read_number<real_type>(
                    t, 0, t.size(), at_line(this->line_num_));
        }

        // Set number of atoms
        this->get_line(line);
        const std::size_t n = read_number<std::size_t>(
                line, 0, line.size(), at_line(this->line_num_));

        // Set atoms
        for(std::size_t i=0; i<n; ++i)
        {
            this->get_line(line);
            frame.lines.push_back(this->read_gro(line));
        }

        // Set box
        std::getline(ifstrm_, line);
        std::istringstream iss(line);
        iss >> frame.box[0] >> frame.box[1] >> frame.box[2];
        if(iss.fail())
        {
            source_location src(this->filename_, line, 0, line.size(), this->line_num_);
            log(log_level::error, "GROReader: while reading box information:\n",
                                   src, "invalid number appeared");
            std::terminate();
        }

        return frame;
    }

  private:

    void get_line(std::stirng& line)
    {
        std::getline(ifstrm_, line);
        this->line_num_++;
        return;
    }

    line_type read_gro(const std::string& line)
    {
        source_location src(this->filename_, line, 0, 0, this->line_num_)

        line_type atom;
        atom.residue_id   = read_number<std::int32_t>(src, 0, 5);
        atom.residue_name = get_substr(src,  5, 5);
        atom.atom_name    = get_substr(src, 10, 5);
        atom.atom_id      = read_number<std::int32_t>(src, 15, 5);
        atom.position[0]  = read_number<double>(src, 20, 8);
        atom.position[1]  = read_number<double>(src, 28, 8);
        atom.position[2]  = read_number<double>(src, 36, 8);
        atom.velocity[0]  = read_number<double>(src, 44, 8);
        atom.velocity[1]  = read_number<double>(src, 52, 8);
        atom.velocity[2]  = read_number<double>(src, 60, 8);

        return atom;
    }


  private:
    std::size_t line_num_;
    std::string filename_;
    std::ifstream ifstrm_;
};

} // jarngreipr
#endif //JARNGREIPR_GRO_READER_HPP
