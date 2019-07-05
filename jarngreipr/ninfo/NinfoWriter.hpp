#ifndef JARNGREIPR_NINFO_WRITER_HPP
#define JARNGREIPR_NINFO_WRITER_HPP
#include <jarngreipr/ninfo/NinfoData.hpp>
#include <jarngreipr/io/log.hpp>
#include <fstream>
#include <iomanip>

namespace jarngreipr
{

template<typename realT>
class NinfoWriter
{
  public:
    using real_type = realT;
    using data_type = NinfoData<real_type>;

  public:

    explicit NinfoWriter(const std::string& fname)
        : filename_(fname), ofstrm_(fname)
    {
        if(!ofstrm_.good())
        {
            log(log_level::error, "NinfoReader: file open error: ", filename_);
            std::terminate();
        }
        ofstrm_.close();
    }

    void write(const data_type& data)
    {
        this->write_block(data.bonds);
        this->write_block(data.angls);
        this->write_block(data.dihds);
        this->write_block(data.aicg13s);
        this->write_block(data.aicg14s);
        this->write_block(data.aicgdihs);
        this->write_block(data.contacts);
        this->write_block(data.basepairs);
        this->write_block(data.basestacks);
        this->write_block(data.pdpwms);
        return;
    }

    template<std::size_t Nu, std::size_t Np, std::size_t Nc, NinfoKind kind>
    void write_block(
        const std::vector<NinfoElement<real_type, Nu, Np, Nc, kind>>& block)
    {
        using ninfo_type = NinfoElement<real_type, Nu, Np, Nc, kind>;
        if(block.empty()){return ;}

        this->ofstrm_.open(this->filename_, std::ios_base::app);
        this->ofstrm_ << "<<<< " << ninfo_type::prefix << '\n';
        for(const auto& ninfo : block)
        {
            this->ofstrm_ << ninfo_type::prefix << ' ';
            this->ofstrm_ << std::setw(6) << std::right << ninfo.id << ' ';
            for(auto& unit  : ninfo.units)
            {
                this->ofstrm_ << std::setw(6) << std::right << unit << ' ';
            }
            for(auto& imp   : ninfo.imps)
            {
                this->ofstrm_ << std::setw(6) << std::right << imp << ' ';
            }
            for(auto& impun : ninfo.impuns)
            {
                this->ofstrm_ << std::setw(6) << std::right << impun << ' ';
            }
            for(auto& coef  : ninfo.coefs)
            {
                this->ofstrm_ << std::setw(9) << std::setprecision(4)
                              << std::fixed << std::showpoint << std::right
                              << coef << ' ';
            }
            this->ofstrm_ << ninfo.suffix << '\n';
        }
        this->ofstrm_ << ">>>>\n";
        ofstrm_.close();
        return;
    }

  private:

    std::string filename_;
    std::ofstream ofstrm_;
};

} // jarngreipr
#endif// JARNGREIPR_NINFO_READER_HPP
