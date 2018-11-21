#define BOOST_TEST_MODULE "test_ninfo_readwrite"
#include <boost/test/included/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <jarngreipr/ninfo/NinfoReader.hpp>
#include <jarngreipr/ninfo/NinfoWriter.hpp>

using test_targets = boost::mpl::list<double, float>;

BOOST_AUTO_TEST_CASE_TEMPLATE(test_ninfo_reader_read_block, Real, test_targets)
{
    // XXX: assuming the test excuted in the `test/` directory!
    jarngreipr::NinfoReader<Real> reader("data/example.ninfo");
    const auto tol = boost::test_tools::tolerance(0.00001);

    {
        const auto& bonds = reader.template read_block<jarngreipr::NinfoKind::bond>();
        BOOST_TEST(bonds.size(), 1);

        const auto& bond = bonds.at(0);
        BOOST_TEST(bond.id           == 1);
        BOOST_TEST(bond.units.at(0)  == 2);
        BOOST_TEST(bond.units.at(1)  == 3);
        BOOST_TEST(bond.imps.at(0)   == 4);
        BOOST_TEST(bond.imps.at(1)   == 5);
        BOOST_TEST(bond.impuns.at(0) == 6);
        BOOST_TEST(bond.impuns.at(1) == 7);
        BOOST_TEST(bond.coefs.at(0)  ==   3.8, tol);
        BOOST_TEST(bond.coefs.at(1)  ==   1.0, tol);
        BOOST_TEST(bond.coefs.at(2)  ==   1.0, tol);
        BOOST_TEST(bond.coefs.at(3)  == 100.0, tol);
    }

    {
        const auto& angls = reader.template read_block<jarngreipr::NinfoKind::angl>();
        BOOST_TEST(angls.size(), 1);

        const auto& angl = angls.at(0);
        BOOST_TEST(angl.id           == 1);
        BOOST_TEST(angl.units.at(0)  == 2);
        BOOST_TEST(angl.units.at(1)  == 3);
        BOOST_TEST(angl.imps.at(0)   == 4);
        BOOST_TEST(angl.imps.at(1)   == 5);
        BOOST_TEST(angl.imps.at(2)   == 6);
        BOOST_TEST(angl.impuns.at(0) == 7);
        BOOST_TEST(angl.impuns.at(1) == 8);
        BOOST_TEST(angl.impuns.at(2) == 9);
        BOOST_TEST(angl.coefs.at(0)  == 120.0, tol);
        BOOST_TEST(angl.coefs.at(1)  ==   1.0, tol);
        BOOST_TEST(angl.coefs.at(2)  ==   1.0, tol);
        BOOST_TEST(angl.coefs.at(3)  ==  20.0, tol);
    }

    {
        const auto& dihds = reader.template read_block<jarngreipr::NinfoKind::dihd>();
        BOOST_TEST(dihds.size(), 1);

        const auto& dihd = dihds.at(0);
        BOOST_TEST(dihd.id           ==  1);
        BOOST_TEST(dihd.units.at(0)  ==  2);
        BOOST_TEST(dihd.units.at(1)  ==  3);
        BOOST_TEST(dihd.imps.at(0)   ==  4);
        BOOST_TEST(dihd.imps.at(1)   ==  5);
        BOOST_TEST(dihd.imps.at(2)   ==  6);
        BOOST_TEST(dihd.imps.at(3)   ==  7);
        BOOST_TEST(dihd.impuns.at(0) ==  8);
        BOOST_TEST(dihd.impuns.at(1) ==  9);
        BOOST_TEST(dihd.impuns.at(2) == 10);
        BOOST_TEST(dihd.impuns.at(3) == 11);
        BOOST_TEST(dihd.coefs.at(0)  == -120.0, tol);
        BOOST_TEST(dihd.coefs.at(1)  ==    1.0, tol);
        BOOST_TEST(dihd.coefs.at(2)  ==    1.0, tol);
        BOOST_TEST(dihd.coefs.at(3)  ==    1.0, tol);
        BOOST_TEST(dihd.coefs.at(4)  ==    0.5, tol);
    }

    {
        const auto& aicg13s = reader.template read_block<jarngreipr::NinfoKind::aicg13>();
        BOOST_TEST(aicg13s.size(), 1);

        const auto& aicg13 = aicg13s.at(0);
        BOOST_TEST(aicg13.id           == 1);
        BOOST_TEST(aicg13.units.at(0)  == 2);
        BOOST_TEST(aicg13.units.at(1)  == 3);
        BOOST_TEST(aicg13.imps.at(0)   == 4);
        BOOST_TEST(aicg13.imps.at(1)   == 5);
        BOOST_TEST(aicg13.imps.at(2)   == 6);
        BOOST_TEST(aicg13.impuns.at(0) == 7);
        BOOST_TEST(aicg13.impuns.at(1) == 8);
        BOOST_TEST(aicg13.impuns.at(2) == 9);
        BOOST_TEST(aicg13.coefs.at(0)  == 7.0,  tol);
        BOOST_TEST(aicg13.coefs.at(1)  == 1.0,  tol);
        BOOST_TEST(aicg13.coefs.at(2)  == 1.0,  tol);
        BOOST_TEST(aicg13.coefs.at(3)  == 0.9,  tol);
        BOOST_TEST(aicg13.coefs.at(4)  == 0.15, tol);
    }

    {
        const auto& aicgdihs = reader.template read_block<jarngreipr::NinfoKind::aicgdih>();
        BOOST_TEST(aicgdihs.size(), 1);

        const auto& aicgdih = aicgdihs.at(0);
        BOOST_TEST(aicgdih.id           ==  1);
        BOOST_TEST(aicgdih.units.at(0)  ==  2);
        BOOST_TEST(aicgdih.units.at(1)  ==  3);
        BOOST_TEST(aicgdih.imps.at(0)   ==  4);
        BOOST_TEST(aicgdih.imps.at(1)   ==  5);
        BOOST_TEST(aicgdih.imps.at(2)   ==  6);
        BOOST_TEST(aicgdih.imps.at(3)   ==  7);
        BOOST_TEST(aicgdih.impuns.at(0) ==  8);
        BOOST_TEST(aicgdih.impuns.at(1) ==  9);
        BOOST_TEST(aicgdih.impuns.at(2) == 10);
        BOOST_TEST(aicgdih.impuns.at(3) == 11);
        BOOST_TEST(aicgdih.coefs.at(0)  == -120.0,  tol);
        BOOST_TEST(aicgdih.coefs.at(1)  ==    1.0,  tol);
        BOOST_TEST(aicgdih.coefs.at(2)  ==    1.0,  tol);
        BOOST_TEST(aicgdih.coefs.at(3)  ==    0.7,  tol);
        BOOST_TEST(aicgdih.coefs.at(4)  ==    0.15, tol);
    }

    {
        const auto& contacts = reader.template read_block<jarngreipr::NinfoKind::contact>();
        BOOST_TEST(contacts.size(), 1);

        const auto& contact = contacts.at(0);
        BOOST_TEST(contact.id           == 1);
        BOOST_TEST(contact.units.at(0)  == 2);
        BOOST_TEST(contact.units.at(1)  == 3);
        BOOST_TEST(contact.imps.at(0)   == 4);
        BOOST_TEST(contact.imps.at(1)   == 5);
        BOOST_TEST(contact.impuns.at(0) == 6);
        BOOST_TEST(contact.impuns.at(1) == 7);
        BOOST_TEST(contact.coefs.at(0)  == 7.0,  tol);
        BOOST_TEST(contact.coefs.at(1)  == 1.0,  tol);
        BOOST_TEST(contact.coefs.at(2)  == 1.0,  tol);
        BOOST_TEST(contact.coefs.at(3)  == 0.3,  tol);
    }

    {
        const auto& pdpwms = reader.template read_block<jarngreipr::NinfoKind::pdpwm>();
        BOOST_TEST(pdpwms.size(), 1);

        const auto& pdpwm = pdpwms.at(0);
        BOOST_TEST(pdpwm.id           == 1);
        BOOST_TEST(pdpwm.units.at(0)  == 2);
        BOOST_TEST(pdpwm.imps.at(0)   == 999);
        BOOST_TEST(pdpwm.impuns.at(0) == 3);
        BOOST_TEST(pdpwm.coefs.at(0)  ==   8.0,  tol);
        BOOST_TEST(pdpwm.coefs.at(1)  ==  60.0,  tol);
        BOOST_TEST(pdpwm.coefs.at(2)  == 120.0,  tol);
        BOOST_TEST(pdpwm.coefs.at(3)  ==  90.0,  tol);
        BOOST_TEST(pdpwm.coefs.at(4)  ==  -0.5,  tol);
        BOOST_TEST(pdpwm.coefs.at(5)  ==   0.5,  tol);
        BOOST_TEST(pdpwm.coefs.at(6)  ==   0.5,  tol);
        BOOST_TEST(pdpwm.coefs.at(7)  ==  -0.5,  tol);
        BOOST_TEST(pdpwm.coefs.at(8)  ==   4.0,  tol);
        BOOST_TEST(pdpwm.coefs.at(9)  ==  -0.4,  tol);
    }
}



BOOST_AUTO_TEST_CASE(test_write_ninfo)
{
    jarngreipr::NinfoReader<double> reader1("data/example.ninfo");
    const auto data1 = reader1.read();

    jarngreipr::NinfoWriter<double> writer ("data/test_output.ninfo");
    writer.write(data1);

    jarngreipr::NinfoReader<double> reader2("data/test_output.ninfo");
    const auto data2 = reader2.read();

    const bool euqality = (data1 == data2);
    BOOST_TEST(euqality);
}
