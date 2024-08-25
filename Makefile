MAKEFLAGS			:= --no-print-directory
.DEFAULT_GOAL		:= all

.DELETE_ON_ERROR:
.SECONDEXPANSION:

UNAME				:= $(shell uname)

#=================================COMPILATION==================================#
CC					:= g++
CPPFLAGS			:= -MP -MMD -I SFML_linux/include -I includes -g3
LIBFSMLFLAG			:= -L SFML_linux/lib -lsfml-graphics -lsfml-window -lsfml-system

#=====================================NAME=====================================#
NAME				:= Gomoku

#==================================DIRECTORIES=================================#
BUILD				:= build

#====================================TARGETS===================================#
SRCS				:=	srcs/main.cpp \
						srcs/Functions.cpp \
						srcs/TextureManager.cpp \
						srcs/Mouse.cpp \
						srcs/Button.cpp \
						srcs/Menu.cpp \
						srcs/Settings.cpp \
						srcs/ModeMenu.cpp \
						srcs/Game.cpp \
						srcs/Grid.cpp \
						srcs/ToggleButton.cpp \
						srcs/Select.cpp \
						srcs/Player.cpp \
						srcs/End.cpp \

OBJS 				:= ${SRCS:srcs/%.cpp=$(BUILD)/%.o}
DEPS				:= $(SRCS:srcs/%.cpp=$(BUILD)/%.d)
DIRS				:= $(BUILD)

#====================================COLORS====================================#
NOC					:= \033[0m
BOLD				:= \033[1m
UNDERLINE			:= \033[4m
BLINK				:= \e[5m
BLACK				:= \033[1;30m
RED					:= \e[1m;31m;
GREEN				:= \e[1m;32m;
YELLOW				:= \e[1m;33m;
RED					:= \e[1m\e[38;5;196m
GREEN				:= \e[1m\e[38;5;76m
YELLOW				:= \e[1m\e[38;5;220m
BLUE				:= \e[1m\e[38;5;33m
VIOLET				:= \033[1;35m
CYAN				:= \033[1;36m
WHITE				:= \033[1;37m

#=================================COUNTER UTILS================================#
NB_COMPIL			:= 0

ifndef	RECURSIVE
TOTAL_COMPIL		:= $(shell expr $$(make -n RECURSIVE=1 | grep $(CC) | wc -l) - 1)
endif
ifndef TOTAL_COMPIL
TOTAL_COMPIL		:= $(words $(OBJS))
endif

#=====================================RULES====================================#
$(DIRS):
	@mkdir $@

$(OBJS) : $(BUILD)/%.o : srcs/%.cpp | $$(@D)
	$(if $(filter $(NB_COMPIL),0), @echo "$(BLUE)Compiling$(NOC)")
	$(eval NB_COMPIL=$(shell expr $(NB_COMPIL) + 1))
	@echo "$(WHITE)[$(NB_COMPIL)/$(TOTAL_COMPIL)] $(VIOLET)Compiling $< $(NOC)"
	@$(CC) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(BLUE)Creation of binary$(NOC)"
	@g++ $^ $(LIBFSMLFLAG) -o $(NAME)
	@echo "$(GREEN)Done$(NOC)"

all : $(NAME)

clean :
	@echo "$(RED)Deleting objects$(NOC)"
	@rm -rf $(BUILD) 2>/dev/null || echo -n

fclean : clean
	@echo "$(RED)Deleting binary$(NOC)"
	@rm -f $(NAME)

re :
	@clear
	@make fclean
	@make

run: $(NAME)
	@echo "$(BLUE)Launch game$(NOC)"
	@export LD_LIBRARY_PATH=SFML_linux/lib/ && ./$(NAME) && echo "$(GREEN)Have a nice day :)$(NOC)" || echo "$(RED)Problem$(NOC)"

runval: $(NAME)
	@echo "$(BLUE)Debug$(NOC)"
	@export LD_LIBRARY_PATH=SFML_linux/lib/ && valgrind --suppressions=vsupp ./$(NAME)

runallval: $(NAME)
	@echo "$(BLUE)Debug$(NOC)"
	@export LD_LIBRARY_PATH=SFML_linux/lib/ && valgrind --suppressions=vsupp --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

.PHONY : $(NAME) all clean fclean re run runval runallval

-include $(DEPS)
