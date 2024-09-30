MAKEFLAGS			:= --no-print-directory
.DEFAULT_GOAL		:= all

.DELETE_ON_ERROR:
.SECONDEXPANSION:

#=================================COMPILATION==================================#
CC					:= g++
CPPFLAGS			:= -MP -MMD -I SFML_linux/include -I srcs -g3
LIBFSMLFLAG			:= -L SFML_linux/lib -lsfml-graphics -lsfml-window -lsfml-system

#=====================================NAME=====================================#
NAME				:= Gomoku

#==================================DIRECTORIES=================================#
BUILD				:= build

#====================================TARGETS===================================#
SRCS				:=	srcs/main.cpp \
						srcs/utils/Mouse.cpp \
						srcs/utils/Functions.cpp \
						srcs/utils/TextureManager.cpp \
						srcs/ui/Select.cpp \
						srcs/ui/Button.cpp \
						srcs/ui/ToggleButton.cpp \
						srcs/screen/End.cpp \
						srcs/screen/Menu.cpp \
						srcs/screen/Game.cpp \
						srcs/screen/Settings.cpp \
						srcs/screen/ModeMenu.cpp \
						srcs/game/Grid.cpp \
						srcs/game/Bbox.cpp \
						srcs/game/Player.cpp \
						srcs/game/BitBoard.cpp \
						srcs/game/BoardState.cpp \
						srcs/game/PlayerInfo.cpp \
						srcs/game/BboxManager.cpp \
						srcs/AI/AI.cpp \
						srcs/AI/AI_easy.cpp \
						srcs/AI/AI_medium.cpp \
						srcs/AI/AI_random.cpp \
						srcs/AI/Evaluation.cpp \
						srcs/AI/AI_betterRandom.cpp \

OBJS 				:= ${SRCS:srcs/%.cpp=$(BUILD)/%.o}
DEPS				:= $(SRCS:srcs/%.cpp=$(BUILD)/%.d)
DIRS				:= $(sort $(shell dirname $(BUILD) $(OBJS)))

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

runvalthread: $(NAME)
	@echo "$(BLUE)Debug$(NOC)"
	@export LD_LIBRARY_PATH=SFML_linux/lib/ && valgrind --trace-children=yes --track-origins=yes --suppressions=vsupp ./$(NAME)

runallval: $(NAME)
	@echo "$(BLUE)Debug$(NOC)"
	@export LD_LIBRARY_PATH=SFML_linux/lib/ && valgrind --suppressions=vsupp --leak-check=full --show-leak-kinds=all --track-origins=yes --error-limit=no ./$(NAME)

.PHONY : $(NAME) all clean fclean re run runval runallval

-include $(DEPS)
