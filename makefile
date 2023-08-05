Name = Webserv

CC = c++

CFLAGS			= -Wall -Wextra -Werror -std=c++98 
INCLUDES 		= -I./inc/
SRCDIR 			= src
OBJDIR 			= build
SRC 		= $(shell find $(SRCDIR) -name "*.cpp")
OBJ 		= $(SRC:%.cpp=$(OBJDIR)/%.o)
SRC_HEADERS = $(shell find inc -name "*.hpp")

all : $(Name)

$(Name) : $(OBJ)
	echo "Compiling..."
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o : %.cpp $(SRC_HEADERS)
	echo "Creating object directory..."
	@mkdir -p $(@D)
	echo "building $@..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(Name)

re : fclean all

.PHONY : all fclean re clean
