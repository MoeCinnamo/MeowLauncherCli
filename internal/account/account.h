#ifndef ACCOUNT_H
#define ACCOUNT_H

/*
 * Local offline accounts. Persisted to ~/.meowlaunch/data/users.json.
 */

/* Add a local account with the given username. Prints feedback via stdout.
 * Returns 0 on success, nonzero on error. */
int Account_Local_Add(const char *name);

/* Remove a local account by username. Returns 0 on success, nonzero when
 * the account does not exist. */
int Account_Local_Del(const char *name);

/* List all local accounts. Returns 0 on success. */
int Account_Local_Show(void);

#endif
