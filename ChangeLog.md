# Release 0.5

Features:
* The datafinder can search in multiple paths for a configuration by priority. This is a convenience function for people using the w32bindkeys API.
* The used WIN32 hook to receive the key combinations has been substaintially updated.
  * Multiple hooks are used
  * Locking the session would crash the hooks. This is avoided.
  * The tracked key combinations are periodically reset
* Important classes for the API have been refactored to be fully object oriented. This avoid code duplication for users of the API. The classes are:
  * wbk_kc_t
  * wbk_kc_sys_t
  * wbk_kbman_t
