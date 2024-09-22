idf_component_register(
	# SRCS表示要包含的`.c`文件
	SRCS 
	"key_main.c"
	"Dir/Ketboard_Driver.c"
	"Dir/Motor_Driver.c"
	"Dir/Voice_Driver.c"
	
	# INCLUDE_DIRS表示要包含的`.h`文件的目录
    INCLUDE_DIRS "./Dir"
	             "./APP"
				 "./Com"
)