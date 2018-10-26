#ifndef JARNGREIPR_PDB_READER_HPP
#define JARNGREIPR_PDB_READER_HPP
#include <jarngreipr/pdb/PDBAtom.hpp>
#include <jarngreipr/pdb/PDBChain.hpp>
#include <jarngreipr/io/read_number.hpp>
#include <mjolnir/util/throw_exception.hpp>
#include <fstream>
#include <sstream>

namespace jarngreipr
{

// lazy pdb reader. assuming there are only one model.
template<typename realT>
class PDBReader
{
  public:
    using real_type  = realT;
    using atom_type  = PDBAtom<real_type>;
    using chain_type = PDBChain<real_type>;

  public:

    explicit PDBReader(const std::string& fname)
        : line_num_(0), filename_(fname), ifstrm_(fname)
    {
        if(!ifstrm_.good())
        {
            write_error(std::cerr, "PDBReader: file open error: ", filename_);
            std::exit(EXIT_FAILURE);
        }
    }

    bool is_eof() {this->ifstrm_.peek(); return this->ifstrm_.eof();}
    void rewind() {this->ifstrm_.seekg(0, std::ios::beg);}

    chain_type const& read_chain(const char id)
    {
        const auto found = std::find_if(chains_.begin(), chains_.end(),
            [id](const chain_type& ch) noexcept -> bool {
                return ch.chain_id() == id;
            });
        if(found != this->chains_.end())
        {
            return *found;
        }

        while(!this->ifstrm_.eof())
        {
            this->chains_.push_back(this->read_next_chain());
            if(this->chains_.back().chain_id() == id)
            {
                return this->chains_.back();
            }
        }

        write_error(std::cerr, "PDBReader: ", this->filename_,
                    " does not contain chain ", id, '.');
        std::exit(EXIT_FAILURE);
    }

  private:

    // lazy functions.
    atom_type read_atom(const std::string& line)
    {
        const auto ln  = at_line(this->line_num_);
        const auto msg = std::string("while reading pdb ATOM in ") + this->filename_;

        if(get_substr(line, 0, 6, msg, ln) != "ATOM  ")
        {
            write_error(std::cerr, "[internal erorr] PDBReader: non ATOM line");
            write_underline(std::cerr, line, 0, 6, '^', ln);
            std::exit(EXIT_FAILURE);
        }

        atom_type atm;

        // these values are required
        atm.atom_id      = read_number<std::int32_t>(line, 6, 5, msg, ln);
        atm.atom_name    = get_substr (line, 12, 4, msg, ln);
        atm.altloc       = get_char_at(line, 16,    msg, ln);
        atm.residue_name = get_substr (line, 17, 3);
        atm.chain_id     = get_char_at(line, 21,    msg, ln);
        atm.residue_id   = read_number<std::int32_t>(line, 22, 4, msg, ln);
        atm.icode        = get_char_at(line, 26,    msg, ln);
        atm.position[0]  = read_number<real_type>(line, 30, 8, msg, ln);
        atm.position[1]  = read_number<real_type>(line, 38, 8, msg, ln);
        atm.position[2]  = read_number<real_type>(line, 46, 8, msg, ln);

        // allow files that lack the following values. default values are
        atm.occupancy          = 0.0;
        atm.temperature_factor = 0.0;
        atm.element            = "  ";
        atm.charge             = "  ";

        //XXX if the column exists, it should be a valid value.
        if(line.size() < 60) {return atm;}
        atm.occupancy          = read_number<real_type>(line, 54, 6, msg, ln);
        if(line.size() < 66) {return atm;}
        atm.temperature_factor = read_number<real_type>(line, 60, 6, msg, ln);
        if(line.size() < 78) {return atm;}
        atm.element            = get_substr(line, 76, 2, msg, ln);
        if(line.size() < 80) {return atm;}
        atm.charge             = get_substr(line, 78, 2, msg, ln);

        return atm;
    }

    chain_type read_next_chain()
    {
        std::vector<atom_type> atoms;
        while(!this->is_eof())
        {
            std::string line;
            std::getline(ifstrm_, line);
            this->line_num_ += 1;

            if(line.size() >= 6 && line.substr(0, 6) == "ATOM  ")
            {
                atoms.push_back(this->read_atom(line));
            }
            else if(line.size() >= 3 && line.substr(0, 3) == "TER")
            {
                return chain_type(std::move(atoms));
            }
            else if(line.size() >= 6 && line.substr(0, 6) == "ENDMDL")
            {
                return chain_type(std::move(atoms));
            }
            continue;
        }
        if(!atoms.empty())
        {
            return chain_type(std::move(atoms));
        }
        write_error(std::cerr, "PDBReader: ", this->filename_,
                    " does not contain chain any more.");
        std::exit(EXIT_FAILURE);
    }

  private:

    std::size_t line_num_;
    std::string filename_;
    std::ifstream ifstrm_;

    // store chains already read
    std::vector<chain_type> chains_;
};

} // jarngreipr
#endif// JARNGREIPR_PDB_READER_HPP
