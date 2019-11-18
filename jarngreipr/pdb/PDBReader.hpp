#ifndef JARNGREIPR_PDB_READER_HPP
#define JARNGREIPR_PDB_READER_HPP
#include <jarngreipr/pdb/PDBAtom.hpp>
#include <jarngreipr/pdb/PDBChain.hpp>
#include <jarngreipr/util/read_number.hpp>
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
            log(log_level::error, "PDBReader: file open error: ", filename_, '\n');
            std::terminate();
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

        log(log_level::error, "PDBReader: file \"", filename_, "\" does not "
                              "contain chain ", id, ".\n");
        std::terminate();
    }

  private:

    // lazy functions.
    atom_type read_atom(const std::string& line)
    {
        source_location src(this->filename_, line, 0, 6, this->line_num_);
        if(get_substr(src, 0, 6) != "ATOM  ")
        {
            log(log_level::error, "internal error: not an ATOM line\n", src,
                                  "prefix is not \"ATOM\"\n");
            std::terminate();
        }

        atom_type atm;

        // these values are required
        atm.atom_id      = read_number<std::int32_t>(src, 6, 5);
        atm.atom_name    = get_substr (src, 12, 4);
        atm.altloc       = get_char_at(src, 16   );
        atm.residue_name = get_substr (src, 17, 3);
        atm.chain_id     = get_char_at(src, 21   );
        atm.residue_id   = read_number<std::int32_t>(src, 22, 4);
        atm.icode        = get_char_at(src, 26   );
        atm.position[0]  = read_number<real_type>(src, 30, 8);
        atm.position[1]  = read_number<real_type>(src, 38, 8);
        atm.position[2]  = read_number<real_type>(src, 46, 8);

        // allow files that lack the following values. default values are
        atm.occupancy          = 0.0;
        atm.temperature_factor = 0.0;
        atm.element            = "  ";
        atm.charge             = "  ";

        //XXX if the column exists, it should be a valid value.
        if(line.size() < 60) {return atm;}
        atm.occupancy = read_number<real_type>(src, 54, 6);
        if(line.size() < 66) {return atm;}
        atm.temperature_factor = read_number<real_type>(src, 60, 6);
        if(line.size() < 78) {return atm;}
        atm.element = get_substr(src, 76, 2);
        if(line.size() < 80) {return atm;}
        atm.charge  = get_substr(src, 78, 2);

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
            else if((line.size() >= 3 && line.substr(0, 3) == "TER") ||
                    (line.size() >= 6 && line.substr(0, 6) == "ENDMDL"))
            {
                if(!atoms.empty())
                {
                    log(log_level::info, "PDBReader: read chain ",
                        atoms.front().chain_id, ".\n");
                }
                return chain_type(std::move(atoms));
            }
            continue;
        }
        if(!atoms.empty())
        {
            log(log_level::info, "PDBReader: read chain ",
                atoms.front().chain_id, ".\n");
            return chain_type(std::move(atoms));
        }
        log(log_level::error, "PDBReader: ", filename_, " does not contain ",
                              "chains any more\n");
        std::terminate();
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
