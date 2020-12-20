# Last updated : 2020/09/27, 19:08

CC:=g++
CFLAGS := -Wall -Wextra -O2 -pthread	# -Wall -Wextra : 警告の出力，-pthread : std::thread
LIBS = `pkg-config opencv --cflags --libs` `urg_c-config --cflags --libs` -I /usr/local/include -L /usr/local/lib -lwiringPi

SRC_DIR:=./src
OBJ_DIR:=./obj
INC_DIR:=./inc
SRC:=$(wildcard $(SRC_DIR)/*.cpp)
INC:=$(wildcard $(INC_DIR)/*.hpp)
OBJ:=$(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.cpp=.o)))
DEPS:=$(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.cpp=.d)))

SHARE_SRC_DIR:=../share/src
SHARE_OBJ_DIR:=../share/obj
SHARE_INC_DIR:=../share/inc
SHARE_SRC:=$(wildcard $(SHARE_SRC_DIR)/*.cpp)
SHARE_INC:=$(wildcard $(SHARE_INC_DIR)/*.hpp)
SHARE_OBJ:=$(addprefix $(SHARE_OBJ_DIR)/, $(notdir $(SHARE_SRC:.cpp=.o)))
SHARE_DEPS:=$(addprefix $(SHARE_OBJ_DIR)/, $(notdir $(SHARE_SRC:.cpp=.d)))

INCLUDE:= -I $(INC_DIR) -I $(SHARE_INC_DIR)

.PHONY: all
.SILENT:
all: $(OBJ) $(SHARE_OBJ)
	$(CC) \
	$(CFLAGS) \
	$(LIBS) \
	$(INCLUDE) \
	$^ \
	-o $@.out > /dev/null

-include $(DEPS) $(SHARE_DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CC) \
	$(CFLAGS) \
	$(LIBS) \
	$(INCLUDE) \
	-c -MMD  -MP -MF $(@:%.o=%.d) $< \
	-o $@ > /dev/null

$(SHARE_OBJ_DIR)/%.o: $(SHARE_SRC_DIR)/%.cpp
	mkdir -p $(SHARE_OBJ_DIR)
	$(CC) \
	$(CFLAGS) \
	$(LIBS) \
	$(INCLUDE) \
	-c -MMD  -MP -MF $(@:%.o=%.d) $< \
	-o $@ > /dev/null


.PHONY: clean
clean:
	rm *.out -f
	rm $(OBJ_DIR) -r -f

.PHONY: sclean
sclean:
	rm $(SHARE_OBJ_DIR) -r -f

.PHONY: touch
touch:
	touch $(SHARE_SRC) $(SHARE_INC) $(SRC) $(INC)
