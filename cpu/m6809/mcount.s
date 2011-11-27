;;;
;;; Copyright 2011 by Brian Dominy <brian@oddchange.com>
;;;
;;; This file is part of FreeWPC.
;;;
;;; FreeWPC is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; FreeWPC is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with FreeWPC; if not, write to the Free Software
;;; Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;;;

.module mcount

; This module is linked in when CONFIG_PROFILE is enabled.  This
; causes gcc to insert a call to "_mcount" at the top of every C
; function.  This gives us the ability to do runtime profiling.

.area .text

_mcount::
#if defined(CONFIG_PROFILE_BPT) && defined(CONFIG_BPT)
	jsr	*bpt_handler
#endif
	rts

