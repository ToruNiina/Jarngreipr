#ifndef JARNGREIPR_PDB_ATOM_HPP
#define JARNGREIPR_PDB_ATOM_HPP
#include <mjolnir/math/math.hpp>
#include <ostream>
#include <istream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>

namespace jarngreipr
{

template<typename realT>
struct PDBAtom
{
    using real_type   = realT;
    using coordinate_type = mjolnir::math::Vector<realT, 3>;

    char         altloc;
    char         icode;
    char         chain_id;
    std::int32_t atom_id;
    std::int32_t residue_id;
    real_type    occupancy;
    real_type    temperature_factor;
    std::string  atom_name; //XXX with whitespaces!
    std::string  residue_name;
    std::string  element;
    std::string  charge;
    coordinate_type position;
};

template<typename charT, typename traits, typename realT>
std::basic_ostream<charT, traits>& operator<<(
    std::basic_ostream<charT, traits>& os, const PDBAtom<realT>& atm)
{
    os << "ATOM  ";
    os << std::right << std::setw(5) << atm.atom_id;
    os << ' ';
    os << std::setw(4) << atm.atom_name;
    os << atm.altloc;
    os << std::left  << std::setw(3) << atm.residue_name;
    os << ' ';
    os << atm.chain_id;
    os << std::right << std::setw(4) << atm.residue_id;
    os << atm.icode;
    os << "   ";
    os << std::right << std::setw(8) << std::fixed << std::setprecision(3)
       << atm.position[0];
    os << std::right << std::setw(8) << std::fixed << std::setprecision(3)
       << atm.position[1];
    os << std::right << std::setw(8) << std::fixed << std::setprecision(3)
       << atm.position[2];
    os << std::right << std::setw(6) << std::fixed << std::setprecision(2)
       << atm.occupancy;
    os << std::right << std::setw(6) << std::fixed << std::setprecision(2)
       << atm.temperature_factor;
    os << "          ";
    os << std::right << std::setw(2) << atm.element;
    os << std::right << std::setw(2) << atm.charge;
    return os;
}

template<typename realT>
std::string to_string(const PDBAtom<realT>& atm)
{
    std::ostringstream oss;
    oss << atm;
    return oss.str();
}

}//jarngreipr
#endif // JARNGREIPR_PDB_ATOM_HPP
