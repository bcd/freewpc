

define(c_task_sleep, [task_sleep_const
	.db	$1
])

define(c_task_kill_gid, [task_kill_gid_const
	.db	$1
])

define(c_task_create, [task_create_const
	.dw	$1
])

define(c_task_create_gid, [task_create_gid_const
	.dw	$1
	.db	$2
])

define(c_task_create_gid1, [task_create_gid1_const
	.dw	$1
	.db	$2
])

;;;define(c_task_recreate_gid, [task_recreate_gid_const
;;;	.dw	$1
;;;	.db	$2
;;;])
;;;
;;;define(c_sys_error, [sys_error_const
;;;	.db	$1
;;;])
;;;
;;;define(c_sound_send, [sound_send_const
;;;	.db	$1
;;;])
;;;
;;;define(c_deff_start, [deff_start_const
;;;	.dw	$1
;;;	.db	$2
;;;])
;;;
;;;define(c_deff_stop, [deff_stop_const
;;;	.dw	$1
;;;])

