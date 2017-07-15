/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "dirloc.h"

/*
 * Seek to an entry in a directory.
 * _seekdir is in telldir.c so that it can share opaque data structures.
 */
void
seekdir(dirp, loc)
	DIR *dirp;
	long loc;
{
	register struct ddloc **prevlp;
	register struct ddloc *lp;
	struct dirent *dp;
	extern long lseek();

	prevlp = (struct ddloc **)&(dirp->dd_ddloc);
	lp = *prevlp;
	while (lp != NULL) {
		if (lp->loc_index == loc)
			break;
		prevlp = &lp->loc_next;
		lp = lp->loc_next;
	}
	if (lp) {
		if (lp->loc_seek != dirp->dd_seek) {
			if (lseek(dirp->dd_fd, lp->loc_seek, 0) == -1) {
				*prevlp = lp->loc_next;
				free(lp);
				return;
			}
			dirp->dd_seek = lp->loc_seek;
			dirp->dd_loc = 0;
			while (dirp->dd_loc < lp->loc_loc) {
				if (!(dp = readdir(dirp))) {
					*prevlp = lp->loc_next;
					free(lp);
					return;
				}
			}
		}
	}
}