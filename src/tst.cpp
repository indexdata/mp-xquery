#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

#include <zorba/zorba.h>
#include <zorba/store_manager.h>
#include <zorba/serializer.h>
#include <zorba/singleton_item_sequence.h>
#include <zorba/zorba_exception.h>


using namespace zorba;

bool
example_1(Zorba* aZorba)
{
    XQuery_t lQuery = aZorba->compileQuery("1+2");

    std::cout << lQuery << std::endl;

    return true;
}

bool
example_2(Zorba* aZorba)
{
    XQuery_t lQuery = aZorba->createQuery();
    std::string path( "/home/adam/proj/marc2bibframe/xbin/zorba3-0.xqy");

    std::unique_ptr<std::istream> qfile;

    qfile.reset( new std::ifstream( path.c_str() ) );

    Zorba_CompilerHints lHints;

    // lQuery->setFileName("http://base/");
    lQuery->setFileName("/home/adam/proj/marc2bibframe/xbin/");

    lQuery->compile(*qfile, lHints);

    zorba::DynamicContext* lDynamicContext = lQuery->getDynamicContext();

    zorba::Item lItem = aZorba->getItemFactory()->createString("http://base/");
    lDynamicContext->setVariable("baseuri", lItem);

    lItem = aZorba->getItemFactory()->createString(
        "/home/adam/proj/yaz/test/marc6.xml");
    lDynamicContext->setVariable("marcxmluri", lItem);

    lItem = aZorba->getItemFactory()->createString("rdfxml");
    lDynamicContext->setVariable("serialization", lItem);

    std::cout << lQuery << std::endl;

    lItem = aZorba->getItemFactory()->createString(
        "/home/adam/proj/yaz/test/marc7.xml");
    lDynamicContext->setVariable("marcxmluri", lItem);

    std::stringstream ss;

    lQuery->execute(ss);

    std::string result = ss.str();

    std::cout << result << std::endl;

    return true;
}

int main(int argc, char **argv)
{
    void* lStore = StoreManager::getStore();
    Zorba *lZorba = Zorba::getInstance(lStore);

    assert(lZorba);

    example_1(lZorba);

    example_2(lZorba);

    lZorba->shutdown();
    StoreManager::shutdownStore(lStore);
    return 0;
}
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

