#ifndef JARNGREIPR_MODEL_3SPN2_HPP
#define JARNGREIPR_MODEL_3SPN2_HPP
#include <jarngreipr/model/CGBead.hpp>
#include <jarngreipr/model/CGModelGenerator.hpp>
#include <jarngreipr/io/log.hpp>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <string>

namespace jarngreipr
{

template<typename realT>
class ThreeSPN2Base final : public CGBead<realT>
{
  public:
    typedef CGBead<realT> base_type;
    typedef typename base_type::real_type       real_type;
    typedef typename base_type::coordinate_type coordinate_type;
    typedef typename base_type::atom_type       atom_type;
    typedef typename base_type::container_type  container_type;

  public:

    ThreeSPN2Base(std::size_t idx, real_type mass, container_type atoms,
                  std::string name, coordinate_type center_of_mass)
        : base_type(idx, mass, std::move(atoms), std::move(name)),
          position_(center_of_mass)
    {}
    ~ThreeSPN2Base() override = default;

    ThreeSPN2Base(const ThreeSPN2Base&) = default;
    ThreeSPN2Base(ThreeSPN2Base&&)      = default;
    ThreeSPN2Base& operator=(const ThreeSPN2Base&) = default;
    ThreeSPN2Base& operator=(ThreeSPN2Base&&)      = default;

    std::string kind() const override {return "3SPN2Base";}

    coordinate_type position() const override {return this->position_;}

  private:

    coordinate_type position_;
};

template<typename realT>
class ThreeSPN2Sugar final : public CGBead<realT>
{
  public:
    typedef CGBead<realT> base_type;
    typedef typename base_type::real_type       real_type;
    typedef typename base_type::coordinate_type coordinate_type;
    typedef typename base_type::atom_type       atom_type;
    typedef typename base_type::container_type  container_type;

  public:

    ThreeSPN2Sugar(std::size_t idx, real_type mass, container_type atoms,
                   std::string name, coordinate_type center_of_mass)
        : base_type(idx, mass, std::move(atoms), std::move(name)),
          position_(center_of_mass)
    {}
    ~ThreeSPN2Sugar() override = default;

    ThreeSPN2Sugar(const ThreeSPN2Sugar&) = default;
    ThreeSPN2Sugar(ThreeSPN2Sugar&&)      = default;
    ThreeSPN2Sugar& operator=(const ThreeSPN2Sugar&) = default;
    ThreeSPN2Sugar& operator=(ThreeSPN2Sugar&&)      = default;

    std::string kind() const override {return "3SPN2Sugar";}

    coordinate_type position() const override {return this->position_;}

  private:

    coordinate_type position_;
};

template<typename realT>
class ThreeSPN2Phosphate final : public CGBead<realT>
{
  public:
    typedef CGBead<realT> base_type;
    typedef typename base_type::real_type       real_type;
    typedef typename base_type::coordinate_type coordinate_type;
    typedef typename base_type::atom_type       atom_type;
    typedef typename base_type::container_type  container_type;

  public:

    ThreeSPN2Phosphate(std::size_t idx, real_type mass, container_type atoms,
                       std::string name, coordinate_type center_of_mass)
        : base_type(idx, mass, std::move(atoms), std::move(name)),
          position_(center_of_mass)
    {}
    ~ThreeSPN2Phosphate() override = default;

    ThreeSPN2Phosphate(const ThreeSPN2Phosphate&) = default;
    ThreeSPN2Phosphate(ThreeSPN2Phosphate&&)      = default;
    ThreeSPN2Phosphate& operator=(const ThreeSPN2Phosphate&) = default;
    ThreeSPN2Phosphate& operator=(ThreeSPN2Phosphate&&)      = default;

    std::string kind() const override {return "3SPN2Phosphate";}

    coordinate_type position() const override {return this->position_;}

  private:

    coordinate_type position_;
};

template<typename realT>
class ThreeSPN2Generator final : public CGModelGeneratorBase<realT>
{
  public:
    using base_type        = CGModelGeneratorBase<realT>;
    using real_type        = typename base_type::real_type;
    using cg_chain_type    = typename base_type::cg_chain_type;
    using pdb_chain_type   = typename base_type::pdb_chain_type;
    using pdb_atom_type    = typename pdb_chain_type::atom_type;
    using pdb_residue_type = typename pdb_chain_type::const_residue_range;
    using coordinate_type  = typename pdb_atom_type::coordinate_type;

    enum class BeadKind
    {
        Phosphate,
        Sugar,
        Base
    };

  public:

    explicit ThreeSPN2Generator(toml::value mass): masses_(std::move(mass)) {}
    ~ThreeSPN2Generator() override = default;

    cg_chain_type
    generate(const pdb_chain_type& pdb, const std::size_t offset) const override
    {
        cg_chain_type retval(std::string(1, pdb.chain_id()));

        std::string base_kind;
        std::vector<PDBAtom<realT>> phosphate;
        std::vector<PDBAtom<realT>> sugar;
        std::vector<PDBAtom<realT>> base;

        const auto& mass_3SPN2 = toml::find(this->masses_, "3SPN2");

        for(std::size_t i=0; i<pdb.residues_size(); ++i)
        {
            const auto residue_id = pdb.residue_at(i).empty() ? 0 :
                                    pdb.residue_at(i).at(0).residue_id;

            std::tie(base_kind, phosphate, sugar, base) = split_PSB(pdb, i);
            if(i != 0 && phosphate.size() != 5)
            {
                log(log_level::error, "3SPN2: invalid number of atoms in "
                    "phosphate residue ", residue_id, " in chain ", pdb.chain_id(), "\n");
                std::terminate();
            }
            if(sugar.size() != 6u)
            {
                log(log_level::error, "3SPN2: invalid number of atoms in "
                    "sugar residue ", residue_id, " in chain ", pdb.chain_id(), "\n");
                std::terminate();
            }
            if(base.empty())
            {
                log(log_level::error, "3SPN2: no base atoms given in "
                    "residue ", residue_id, " of chain ", pdb.chain_id(), "\n");
                std::terminate();
            }

            const auto P = this->calc_center_of_mass(phosphate);
            const auto S = this->calc_center_of_mass(sugar);
            const auto B = this->calc_center_of_mass(base);

            if(i != 0)
            {
                retval.push_back(std::make_shared<ThreeSPN2Phosphate<real_type>
                    >(3*i-1 + offset, toml::find<real_type>(mass_3SPN2, "P"),
                      phosphate, "P", P));
            }
            retval.push_back(std::make_shared<ThreeSPN2Sugar<real_type>
                    >(3*i   + offset, toml::find<real_type>(mass_3SPN2, "S"),
                      sugar, "S", S));
            retval.push_back(std::make_shared<ThreeSPN2Base <real_type>
                    >(3*i+1 + offset, toml::find<real_type>(mass_3SPN2, base_kind),
                      base, base_kind, B));
        }
        return retval;
    }

  private:

    std::tuple<std::string, std::vector<pdb_atom_type>,
               std::vector<pdb_atom_type>, std::vector<pdb_atom_type>>
    split_PSB(const pdb_chain_type& pdb, const std::size_t i) const
    {
        //                          _
        //                 O5'       \ this part corresponds to residue 1
        //                 |         |
        //              __/          |
        //       Base__/  |          |
        //             \__|          |
        //          _      \         |
        //         /        O3'     _/
        // this is |        |        \ this part corresponds to residue 2
        // CG P    |  OP2 = P == OP1 |
        // bead    |        |        |
        //         \_       O5'      |
        //                  |        |
        //              __ /         |
        //       Base__/  |          |
        //             \__|          |
        //                 \         |
        //                  O3'     _/

        std::vector<pdb_atom_type> phosphate;
        std::vector<pdb_atom_type> sugar;
        std::vector<pdb_atom_type> base;

        if(i != 0)
        {
            const auto resid_prev = pdb.residue_at(i).empty() ? 0 :
                                    pdb.residue_at(i).at(0).residue_id;
            for(const auto& atom : pdb.residue_at(i-1))
            {
                const auto name = remove_whitespaces(atom.atom_name);
                if(name == "O3'" || name == "O3*")
                {
                    phosphate.push_back(atom);
                }
            }
            if(phosphate.empty())
            {
                log(log_level::error, "3SPN2: residue ", resid_prev, " in chain ",
                    pdb.chain_id(), " does not have O3' atom\n");
                std::terminate();
            }
        }

        for(const auto& atom : pdb.residue_at(i))
        {
            const auto name = remove_whitespaces(atom.atom_name);
            if(atom_kind_.count(name) == 0)
            {
                log(log_level::error, "3SPN2: unrecognized DNA atom appeares\n");
                log(log_level::error, atom, '\n');
                std::terminate();
            }
            switch(atom_kind_.at(name))
            {
                case BeadKind::Phosphate:
                {
                    if(name != "O3'" && name != "O3*")
                    {
                        // skip O3' atom in the residue i.
                        // use O3' of i-1 th residue instead.
                        phosphate.push_back(atom);
                    }
                    break;
                }
                case BeadKind::Sugar:
                {
                    sugar.push_back(atom);
                    break;
                }
                case BeadKind::Base:
                {
                    base.push_back(atom);
                    break;
                }
            }
        }

        // assuming residue names are something like "DA"
        const auto base_kind =
            this->remove_whitespaces(base.front().residue_name).substr(1, 1);

        assert(base_kind == "A" || base_kind == "T" ||
               base_kind == "C" || base_kind == "G");

        return std::make_tuple(base_kind, phosphate, sugar, base);
    }

    coordinate_type calc_center_of_mass(const std::vector<pdb_atom_type>& atoms) const
    {
        coordinate_type com(0.0, 0.0, 0.0);
        real_type denom = 0.0;
        for(const auto& atom : atoms)
        {
            // assuming atom.name conforms the wwPDB 3 international standard.
            log(log_level::debug, "atom_name = ", atom.atom_name, ", atom = ",
                                  atom.atom_name.substr(1, 1), '\n');
            const auto m = toml::find(masses_, atom.atom_name.substr(1, 1)).as_floating();
            com   += m * atom.position;
            denom += m;
        }
        if(!atoms.empty())
        {
            com /= denom;
        }
        return com;
    }

    std::string remove_whitespaces(const std::string& str) const
    {
        std::string retval = str;
        while(retval.back() == ' ' || retval.back() == '\t')
        {
            retval.pop_back();
        }
        while(retval.front() == ' ' || retval.front() == '\t')
        {
            retval.erase(retval.begin());
        }
        return retval;
    }

  private:

    toml::value masses_;
    std::map<std::string, BeadKind> atom_kind_ = {
        //----------------------------
        {"O5'", BeadKind::Phosphate},
        {"C5'", BeadKind::Sugar},
        {"C4'", BeadKind::Sugar},
        {"O4'", BeadKind::Sugar},
        {"C3'", BeadKind::Sugar},
        {"O3'", BeadKind::Phosphate},
        {"C2'", BeadKind::Sugar},
        {"C1'", BeadKind::Sugar},
        //----------------------------
        {"O5*", BeadKind::Phosphate},
        {"C5*", BeadKind::Sugar},
        {"C4*", BeadKind::Sugar},
        {"O4*", BeadKind::Sugar},
        {"C3*", BeadKind::Sugar},
        {"O3*", BeadKind::Phosphate},
        {"C2*", BeadKind::Sugar},
        {"C1*", BeadKind::Sugar},
        //----------------------------
        {"N1",  BeadKind::Base},
        {"C2",  BeadKind::Base},
        {"O2",  BeadKind::Base},
        {"N2",  BeadKind::Base},
        {"N3",  BeadKind::Base},
        {"C4",  BeadKind::Base},
        {"N4",  BeadKind::Base},
        {"C5",  BeadKind::Base},
        {"C6",  BeadKind::Base},
        {"N9",  BeadKind::Base},
        {"C8",  BeadKind::Base},
        {"O6",  BeadKind::Base},
        {"N7",  BeadKind::Base},
        {"N6",  BeadKind::Base},
        {"O4",  BeadKind::Base},
        {"C7",  BeadKind::Base},
        //----------------------------
        {"P",   BeadKind::Phosphate},
        {"OP1", BeadKind::Phosphate},
        {"OP2", BeadKind::Phosphate},
        {"O1P", BeadKind::Phosphate},
        {"O2P", BeadKind::Phosphate},
        {"OP3", BeadKind::Phosphate}
    };
};

} // jarngreipr
#endif /*JARNGREIPR_MODEL_3SPN2*/
