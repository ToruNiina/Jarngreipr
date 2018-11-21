#ifndef JARNGREIPR_NINFO_READER_HPP
#define JARNGREIPR_NINFO_READER_HPP
#include <jarngreipr/ninfo/NinfoData.hpp>
#include <jarngreipr/io/read_number.hpp>
#include <fstream>

namespace jarngreipr
{

template<typename realT>
class NinfoReader
{
  public:
    using real_type = realT;
    using data_type = NinfoData<real_type>;

    template<NinfoKind kind>
    using ninfo_t = typename ninfo_type_of<kind, real_type>::type;

  public:

    explicit NinfoReader(const std::string& fname)
        : line_num_(0), filename_(fname), ifstrm_(fname)
    {
        if(!ifstrm_.good())
        {
            write_error(std::cerr, "NinfoReader: file open error: ", filename_);
            std::exit(EXIT_FAILURE);
        }
    }

    bool is_eof() {this->ifstrm_.peek(); return this->ifstrm_.eof();}
    void rewind() {this->ifstrm_.seekg(0, std::ios::beg);}

    data_type const& read()
    {
        // read_block function stores the data into the internal stroage.
        this->read_block<NinfoKind::bond     >();
        this->read_block<NinfoKind::angl     >();
        this->read_block<NinfoKind::dihd     >();
        this->read_block<NinfoKind::aicg13   >();
        this->read_block<NinfoKind::aicg14   >();
        this->read_block<NinfoKind::aicgdih  >();
        this->read_block<NinfoKind::contact  >();
        this->read_block<NinfoKind::basepair >();
        this->read_block<NinfoKind::basestack>();
        this->read_block<NinfoKind::pdpwm    >();
        return this->data_;
    }

    template<NinfoKind kind>
    std::vector<ninfo_t<kind>> const& read_block()
    {
        using ninfo_type = ninfo_t<kind>;

        // if the block has already been read, just return the data.
        auto& corresponding_block = get_block<kind>(this->data_);
        if(!corresponding_block.empty())
        {
            return corresponding_block;
        }

        // the data is not found.
        // it might not exist in the file, or has not read yet.
        this->rewind();

        while(!this->is_eof())
        {
            const auto line = this->getline();
            if(line.empty()) {continue;}
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            if(prefix == ninfo_type::prefix)
            {
                corresponding_block.push_back(this->read_ninfo<ninfo_type>(iss));
            }
        }
        return corresponding_block;
    }

  private:

    template<typename ninfoT>
    ninfoT read_ninfo(std::istringstream& iss) const
    {
        ninfoT ninfo;
        iss >> ninfo.id;
        for(auto& unit  : ninfo.units)  {iss >> unit;}
        for(auto& imp   : ninfo.imps)   {iss >> imp;}
        for(auto& impun : ninfo.impuns) {iss >> impun;}
        for(auto& coef  : ninfo.coefs)  {iss >> coef;}
        if(iss.fail())
        {
            write_error(std::cerr, "while reading ninfo ", ninfoT::prefix,
                        " block in ", this->filename_,
                        " some of the column has invalid format");
            write_underline(std::cerr, iss.str(), 0, iss.str().size(), '^',
                            at_line(this->line_num_));
            std::exit(EXIT_FAILURE);
        }

        // if there are no suffix, it does not matter.
        iss >> ninfo.suffix;

        return ninfo;
    }

    std::string getline()
    {
        this->line_num_ += 1;
        std::string line;
        std::getline(this->ifstrm_, line);
        return line;
    }

  private:

    std::size_t line_num_;
    std::string filename_;
    std::ifstream ifstrm_;
    data_type       data_; // store blocks that have already been read
};

} // jarngreipr
#endif// JARNGREIPR_NINFO_READER_HPP