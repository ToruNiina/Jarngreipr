#ifndef JARNGREIPR_IO_GRO_LINE_HPP
#define JARNGREIPR_IO_GRO_LINE_HPP
#include <mjolnir/math/Vector.hpp>
#include <utility>
#include <ostream>
#include <istream>
#include <iomanip>
#include <sstream>
#include <string>

namespace jarngreipr
{

template<typename realT>
struct GROLine
{
    using real_type = realT;
    using coordinate_type = mjolnir::Vector<real_type, 3>;

    std::uint32_t atom_id;
    std::uint32_t residue_id;

    std::string atom_name;
    std::string residue_name;

    coordinate_type position;
    coordinate_type velocity;
};

template<typename realT>
inline GROLine<realT>
make_gro_line(const std::int32_t atm_id = 0,
        const std::int32_t res_id = 0,
        const std::string& atm_name = "AAAAA",
        const std::string& res_name = "RRRRR",
        const mjolnir::Vector<realT, 3>& pos = {0.0, 0.0, 0.0},
        const mjolnir::Vector<realT, 3>& vel = {0.0, 0.0, 0.0},
        const mjolnir::Vector<realT, 3>& box = {0.0, 0.0, 0.0})
{
    return GROLine<realT>{atm_id, res_id, atm_name, 
        res_name, pos, vel, box};
}

template<typename charT, typename traits, typename realT>
std::basic_ostream<charT, traits>& operator<<(
    std::basic_ostream<charT, traits>& os, const GROLine<realT>& gro_line)
{
    os << std::setw(5) << std::right << gro_line.residue_id;
    os << std::setw(5) << std::left  << gro_line.residue_name;
    os << std::setw(5) << std::right << gro_line.atom_name;
    os << std::setw(5) << std::right << gro_line.atom_id;
    os << std::setw(8) << std::right << std::setprecision(3)
        << std::fixed << gro_line.position[0];
    os << std::setw(8) << std::right << std::setprecision(3)
        << std::fixed << gro_line.position[1];
    os << std::setw(8) << std::right << std::setprecision(3)
        << std::fixed << gro_line.position[2];
    os << std::setw(8) << std::right << std::setprecision(4)
        << std::fixed << gro_line.velocity[0];
    os << std::setw(8) << std::right << std::setprecision(4)
        << std::fixed << gro_line.velocity[1];
    os << std::setw(8) << std::right << std::setprecision(4)
        << std::fixed << gro_line.velocity[2];

    return os;
}

template<typename charT, typename traits, typename realT>
std::basic_istream<charT, traits>& operator>>(
    std::basic_istream<charT, traits>& is, GROLine<realT>& gro_line)
{
    std::string line;
    std::getline(is, line);

    gro_line.residue_id   = std::stoi(line.substr( 0, 5));
    gro_line.residue_name = line.substr( 5, 10);
    gro_line.atom_name    = line.substr(10, 15);
    gro_line.atom_id      = std::stoi(line.substr(15, 20));
    gro_line.position[0]  = std::stod(line.substr(20, 28));
    gro_line.position[1]  = std::stod(line.substr(28, 36));
    gro_line.position[2]  = std::stod(line.substr(36, 44));
    gro_line.velocity[0]  = std::stod(line.substr(44, 52));
    gro_line.velocity[1]  = std::stod(line.substr(52, 60));
    gro_line.velocity[2]  = std::stod(line.substr(60, 68));

    return is;
}

} // jarngreipr
#endif //JARNGREIPR_IO_GRO_LINE_HPP
