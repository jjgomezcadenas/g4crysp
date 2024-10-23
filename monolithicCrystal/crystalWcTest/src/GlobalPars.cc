#include "GlobalPars.hh"

GlobalPars* GlobalPars::instance = nullptr;

GlobalPars::GlobalPars() {}

GlobalPars* GlobalPars::Instance() {
    if (!instance) {
        instance = new GlobalPars();
    }
    return instance;
}
