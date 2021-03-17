/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACCOUNTS_H_
#define ACCOUNTS_H_

	#include <sys/types.h>
	#include <string.h>

	/**
	 * @brief Maximum length for a user name.
	 */
	#define USERNAME_MAX 20
	
	/**
	 * @brief Maximum length for a password.
	 */
	#define PASSWORD_MAX 10

	/**
	 * @brief User's account information.
	 */
	struct account
	{
		char name[USERNAME_MAX];     /**< User name.       */
		char password[PASSWORD_MAX]; /**< User's password. */
		uid_t uid;                   /**< User's ID.       */
		gid_t gid;                   /**< User's group ID. */
	};


	void generateRandomKey(char * key) {
		int lfsr = 0xACE1u;
		//unsigned bit; 
		unsigned period = 0;
		printf("%s", key); 

		//int keyLength = strlen(key);

		do { /* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
			//bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ^ (lfsr >> 8)) & 1;

			/*for (int i = 0; i < keyLength; i++) {
				key[i] = key[i] ^ (bit & 0xFF);
			}*/

			lfsr =  (lfsr >> 1) | ((lfsr & 0x1) << 31);  
			++period;
		} 
		while(lfsr != 0xACE1u);
	}

	/**
	 * @brief Encrypts a string.
	 * 
	 * @param str String to encode.
	 * @param n   String length.
	 * @param key Encrypting key.
	 */
	extern inline void account_encrypt(char *str, size_t n, char * key)
	{
		printf("%s", key);
		int keyLength = strlen(key);
		generateRandomKey(key);

		for (size_t i = 0; i < n; i++) {
			str[i] = str[i] ^ key[i%keyLength];
		}
		
	}
	
	/**
	 * @brief Decrypts a string.
	 * 
	 * @param str String to encode.
	 * @param n   String length.
	 * @param key Encrypting key.
	 */
	extern inline void account_decrypt(char *str, size_t n, char * key)
	{
		printf("%s", key);
		int keyLength = strlen(key);
		generateRandomKey(key);

		for (size_t i = 0; i < n; i++) {
			str[i] = str[i] ^ key[i%keyLength];
		}
		
	}

#endif /* ACCOUNTS_H_ */
