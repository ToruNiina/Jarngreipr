#ifndef JARNGREIPR_GRO_READER_HPP
#define JARNGREIPR_GRO_READER_HPP
#include <jarngreipr/gro/GROLine.hpp>
#include <jarngreipr/gro/GROFrame.hpp>
#include <jarngreipr/io/read_number.hpp>
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
            write_error(std::cerr, "GROReader: file open error: ", filename_);
            std::exit(EXIT_FAILURE);
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
            write_error("GROReader: while reading box information:");
            write_underline(line, 0, line.size(), '-', at_line(this->line_num_));
            std::exit(EXIT_FAILURE);
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
        const auto ln  = at_line(this->line_num_);
        const auto msg = std::string("while reading gro ATOM in ") + this->filename_;

        line_type atom;
        atom.residue_id   = read_number<std::int32_t>(line, 0, 5, msg, ln);
        atom.residue_name = get_substr(line,  5, 5, msg, ln);
        atom.atom_name    = get_substr(line, 10, 5, msg, ln);
        atom.atom_id      = read_number<std::int32_t>(line, 15, 5, msg, ln);
        atom.position[0]  = read_number<double>(line, 20, 8, msg, ln);
        atom.position[1]  = read_number<double>(line, 28, 8, msg, ln);
        atom.position[2]  = read_number<double>(line, 36, 8, msg, ln);
        atom.velocity[0]  = read_number<double>(line, 44, 8, msg, ln);
        atom.velocity[1]  = read_number<double>(line, 52, 8, msg, ln);
        atom.velocity[2]  = read_number<double>(line, 60, 8, msg, ln);

        return atom;
    }


  private:
    std::size_t line_num_;
    std::string filename_;
    std::ifstream ifstrm_;
};

} // jarngreipr
#endif //JARNGREIPR_GRO_READER_HPP
