#ifndef JARNGREIPR_PDB_WRITER_HPP
#define JARNGREIPR_PDB_WRITER_HPP
#include <jarngreipr/pdb/PDBAtom.hpp>
#include <jarngreipr/pdb/PDBChain.hpp>
#include <fstream>
#include <sstream>

namespace jarngreipr
{

template<typename realT>
class PDBWriter
{
  public:
    typedef PDBAtom<realT>  atom_type;
    typedef PDBChain<realT> chain_type;
    typedef std::vector<chain_type> model_type;

  public:

    explicit PDBWriter(const std::string& fname)
        : filename_(fname), ofstrm_(fname)
    {
        if(!ofstrm_.good())
        {
            log::error("PDBWriter: file open error: ", filename_, '\n');
            std::terminate();
        }
    }

    void write_chain(const chain_type& chain)
    {
        for(const auto& atom : chain)
        {
            this->write_atom(atom);
        }
        ofstrm_ << "TER\n";
        return;
    }

    void write_atom(const atom_type& chain)
    {
        this->ofstrm_ << "ATOM  ";
        this->ofstrm_ << std::right << std::setw(5) << atm.atom_id;
        this->ofstrm_ << ' ';
        this->ofstrm_ << std::setw(4) << atm.atom_name;
        this->ofstrm_ << atm.altloc;
        this->ofstrm_ << std::left  << std::setw(3) << atm.residue_name;
        this->ofstrm_ << ' ';
        this->ofstrm_ << atm.chain_id;
        this->ofstrm_ << std::right << std::setw(4) << atm.residue_id;
        this->ofstrm_ << atm.icode;
        this->ofstrm_ << "   ";
        this->ofstrm_ << std::right << std::setw(8) << std::fixed << std::setprecision(3)
                      << atm.position[0];
        this->ofstrm_ << std::right << std::setw(8) << std::fixed << std::setprecision(3)
                      << atm.position[1];
        this->ofstrm_ << std::right << std::setw(8) << std::fixed << std::setprecision(3)
                      << atm.position[2];
        this->ofstrm_ << std::right << std::setw(6) << std::fixed << std::setprecision(2)
                      << atm.occupancy;
        this->ofstrm_ << std::right << std::setw(6) << std::fixed << std::setprecision(2)
                      << atm.temperature_factor;
        this->ofstrm_ << "          ";
        this->ofstrm_ << std::right << std::setw(2) << atm.element;
        this->ofstrm_ << std::right << std::setw(2) << atm.charge;
        this->ofstrm_ << '\n';
        return;
    }

  private:
    std::string filename_;
    std::ofstream ofstrm_;
};

} // jarngreipr
#endif// JARNGREIPR_PDB_WRITER_HPP
