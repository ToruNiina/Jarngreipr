#ifndef JARNGREIPR_MODEL_CARBON_ALPHA_HPP
#define JARNGREIPR_MODEL_CARBON_ALPHA_HPP
#include <jarngreipr/model/CGBead.hpp>
#include <jarngreipr/model/CGModelGenerator.hpp>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <string>

namespace jarngreipr
{

/*! @brief carbon alpha 1 beads per amino acid model */
template<typename realT>
class CarbonAlpha final : public CGBead<realT>
{
  public:
    typedef CGBead<realT> base_type;
    typedef typename base_type::real_type       real_type;
    typedef typename base_type::coordinate_type coordinate_type;
    typedef typename base_type::atom_type       atom_type;
    typedef typename base_type::container_type  container_type;

  public:

    CarbonAlpha(std::size_t idx, real_type mass, container_type atoms, std::string name)
        : base_type(idx, mass, std::move(atoms), std::move(name))
    {
        if(this->atoms_.empty())
        {
            log::error("CarbonAlpha: initialized with no atoms.\n");
            std::terminate();
        }
        const auto is_ca =
            [](const atom_type& a){return a.atom_name == " CA ";};
        const std::size_t num_ca = std::count_if(
            this->atoms_.cbegin(), this->atoms_.cend(), is_ca);
        if(num_ca == 0)
        {
            log::error("CarbonAlpha: no c-alpha atom exists in a residue.\n");
            log::error(this->atoms_.front(), '\n');
            std::terminate();
        }
        if(num_ca > 1)
        {
            log::error("CarbonAlpha: ", num_ca,
                " c-alpha atoms exist in a residue.\n");
            for(const auto& atm : this->atoms_)
            {
                if(is_ca(atm))
                {
                    log::error(atm, '\n');
                }
            }
            std::terminate();
        }
        this->position_ = std::find_if(
            this->atoms_.cbegin(), this->atoms_.cend(), is_ca)->position;
    }
    ~CarbonAlpha() override = default;

    CarbonAlpha(const CarbonAlpha&) = default;
    CarbonAlpha(CarbonAlpha&&)      = default;
    CarbonAlpha& operator=(const CarbonAlpha&) = default;
    CarbonAlpha& operator=(CarbonAlpha&&)      = default;

    std::string kind() const override {return "CarbonAlpha";}

    coordinate_type position() const override {return this->position_;}

  private:

    coordinate_type position_;
};

template<typename realT>
class CarbonAlphaGenerator final : public CGModelGeneratorBase<realT>
{
  public:
    typedef CGModelGeneratorBase<realT>        base_type;
    typedef typename base_type::real_type      real_type;
    typedef typename base_type::cg_chain_type  cg_chain_type;
    typedef typename base_type::pdb_chain_type pdb_chain_type;

  public:

    explicit CarbonAlphaGenerator(toml::value mass): masses_(std::move(mass)) {}
    ~CarbonAlphaGenerator() override = default;

    cg_chain_type
    generate(const pdb_chain_type& pdb, const std::size_t offset) const override
    {
        const auto& mass_AICG2p = toml::find(this->masses_, "AICG2+");

        CGChain<realT> retval(std::string(1, pdb.chain_id()));
        for(std::size_t i=0; i<pdb.residues_size(); ++i)
        {
            const auto res = pdb.residue_at(i);
            std::vector<PDBAtom<realT>> atoms(res.begin(), res.end());
            const auto name = atoms.front().residue_name;
            retval.push_back(std::make_shared<CarbonAlpha<realT>>(
                    i + offset, toml::find<real_type>(mass_AICG2p, name),
                    std::move(atoms), name));
        }
        return retval;
    }

  private:

    toml::value masses_;
};

} // jarngreipr
#endif /*JARNGREIPR_CARBON_ALPHA*/
