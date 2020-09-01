#ifndef JARNGREIPR_FORCEFIELD_REMOVE_HYDROGENS_HPP
#define JARNGREIPR_FORCEFIELD_REMOVE_HYDROGENS_HPP
#include <jarngreipr/util/log.hpp>
#include <jarngreipr/pdb/PDBAtom.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <cassert>

// utility function mainly used by ForceFieldGenerators

namespace jarngreipr
{

template<typename realT>
std::vector<PDBAtom<realT>> remove_hydrogens(std::vector<PDBAtom<realT>> atoms)
{
    const auto removed = std::remove_if(atoms.begin(), atoms.end(),
        [](const PDBAtom<realT>& atom) -> bool {
            assert(atom.element.size() == 2);
            assert(atom.atom_name.size() == 4);

            // wwPDB v3 conformant (should be right justified, but allow...)
            if(atom.element == " H" || atom.element == "H ") {return true;}
            if(atom.atom_name.at(1) == 'H')
            {
                return true;
            }

            // older or something others
            std::string buf = atom.atom_name;
            while(buf.front() == ' ') {buf.erase(buf.begin());}
            if(buf.front() == 'H')
            {
                if(atom.element == "Hg" || atom.element == "HG" ||
                   atom.element == "Hf" || atom.element == "HF" ||
                   atom.element == "Ho" || atom.element == "HO" ||
                   atom.element == "Hs" || atom.element == "HS")
                {
                    return false;
                }
                log::warn("Atom name starting with H found. "
                                     "Considering it as a hydrogen.\n");
                log::warn("If it is not a hydrogen, "
                                     "add element symbol section.\n");
                log::warn(atom, '\n');
                return true;
            }
            return false;
        });
    atoms.erase(removed, atoms.end());
    return atoms;
}

} // jarngreipr
#endif//JARNGREIPR_FORCEFIELD_REMOVE_HYDROGENS_HPP
