#ifndef JARNGREIPR_FORCEFIELD_EXCLUDED_VOLUME
#define JARNGREIPR_FORCEFIELD_EXCLUDED_VOLUME
#include <jarngreipr/forcefield/ForceFieldGenerator.hpp>
#include <mjolnir/util/get_toml_value.hpp>
#include <extlib/toml/toml.hpp>

namespace jarngreipr
{

template<typename realT>
class ExcludedVolume final : public ForceFieldGenerator<realT>
{
  public:
    typedef ForceFieldGenerator<realT> base_type;
    typedef typename base_type::real_type  real_type;
    typedef typename base_type::bead_type  bead_type;
    typedef typename base_type::chain_type chain_type;

  public:

    ExcludedVolume(const toml::Table& para) : parameters_(para){}
    ~ExcludedVolume() override = default;

    void generate(toml::Table& out,
        const std::vector<chain_type>& chains
        ) const override;

    void generate(toml::Table& out,
        const std::vector<chain_type>& lhs, const std::vector<chain_type>& rhs
        ) const override
    {
        std::cerr << "WARNING: inter-chain(does not include intra-chain) "
                  << "ExcludedVolume is not suppored yet." << std::endl;
        return;
    }

    bool check_beads_kind(const chain_type& chain) const override
    {return true;}

  private:

    toml::Table parameters_;
};

template<typename realT>
void ExcludedVolume<realT>::generate(toml::Table& ff,
        const std::vector<chain_type>& chains) const
{
    if(ff.count("global") == 0)
    {
        ff["global"] = toml::Array();
    }

    toml::Table exv;
    exv["interaction"] = toml::String("Pair");
    exv["potential"]   = toml::String("ExcludedVolume");

    toml::Table particles_within;
    particles_within["bond"]    = 3;
    particles_within["contact"] = 1;
    toml::Table ignore;
    ignore["molecule"] = toml::String("Nothing");
    ignore["particles_within"] = particles_within;
    exv["ignore"] = ignore;

    toml::Table partition;
    partition["type"]   = toml::String("CellList");
    partition["margin"] = 0.5;
    exv["spatial_partition"] = partition;

    const toml::Table& radii = mjolnir::toml_value_at(
            this->parameters_, "radii", "jarngreipr::ExcludedVolume"
            ).template cast<toml::value_t::Table>();

    toml::Array params;
    for(const auto& chain : chains)
    {
        for(const auto& bead : chain)
        {
            toml::Table para;
            para["index"] = bead->index();
            para["radius"] = toml::get<toml::Float>(mjolnir::toml_value_at(
                radii, bead->name(), "jarngreipr::ExcludedVolume"));
            params.push_back(para);
        }
    }
    exv["parameters"] = toml::value(std::move(params));
    exv["epsilon"]    = toml::get<toml::Float>(mjolnir::toml_value_at(
                this->parameters_, "epsilon", "jarngreipr::ExcludedVolume"));

    ff["global"].template cast<toml::value_t::Array>().push_back(exv);
    return;
}

} // jarngreipr
#endif// JARNGREIPR_FORCEFIELD_EXCLUDED_VOLUME
