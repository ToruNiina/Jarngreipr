#ifndef JARNGREIPR_XYZ_READER_HPP
#define JARNGREIPR_XYZ_READER_HPP
#include <jarngreipr/xyz/XYZLine.hpp>
#include <jarngreipr/xyz/XYZFrame.hpp>
#include <jarngreipr/io/read_number.hpp>
#include <mjolnir/util/throw_exception.hpp>
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
    typedef XYZLine<realT>  line_type;
    typedef XYZFrame<realT> frame_type;

  public:
    explicit XYZReader(const std::string& fname)
        : line_num_(0), filename_(fname), ifstrm_(fname)
    {
        if(!ifstrm_.good())
        {
            write_error(std::cerr, "XYZReader: file open error: ", fname);
            std::exit(EXIT_FAILURE);
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

        write_error(std::cerr, "XYZReader: ", this->filename_,
                    " does not contain frame ", idx, '.');
        std::exit(EXIT_FAILURE);
    }

  private:

    frame_type read_next_frame()
    {
        std::string line;
        std::getline(ifstrm_, line);
        this->line_num_++;

        const auto msg = std::string("while reading XYZ file ") + this->fname;
        const std::size_t n = get_number<std::size_t>(
                line, 0, line.size(), msg, at_line(this->line_num_));

        frame_type frame;
        std::getline(ifstrm_, frame.comment);
        for(std::size_t i=0; i<n; ++i)
        {
            std::getline(ifstrm_, line);
            this->line_num_++;

            std::istringstream iss(line);
            line_type xyz;
            iss >> xyz;
            frame.lines.push_back(xyz);
        }
        return frame;
    }

  private:
    std::size_t line_num_;
    std::string filename_;
    std::ifstream ifstrm_;
    std::vector<frame_type> frames_;
};

} // mjolnir
#endif //JARNGREIPR_IO_XYZ_READER
