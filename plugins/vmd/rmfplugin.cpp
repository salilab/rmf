#include "rmfplugin.h"

VMDPLUGIN_API int VMDPLUGIN_init() {
  init_plugins();
  return VMDPLUGIN_SUCCESS;
}
VMDPLUGIN_API int VMDPLUGIN_register(void *v, vmdplugin_register_cb cb) {
  std::cout << "Register" << std::endl;
  (*cb)(v, (vmdplugin_t *)&plugin);
  (*cb)(v, (vmdplugin_t *)&plugin3);
  (*cb)(v, (vmdplugin_t *)&pluginz);
  return 0;
}
VMDPLUGIN_API int VMDPLUGIN_fini() {
  std::cout << "Fini" << std::endl;
  return VMDPLUGIN_SUCCESS;
}
