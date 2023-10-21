OBJS += src/havarg.o src/common.o src/error_methods.o

OUTDIR ?= build
DBGOUTDIR ?= debug

OUT ?= $(OUTDIR)/libhavarg.a
DBGOUT ?= $(DBGOUTDIR)/libhavarg.a

WFLAGS += -Wall -Wextra -Wpedantic -Werror
CFLAGS += 
CFLAGS_FILE ?= compile_flags.txt

# Compile C files
.SUFFIXES: .c .o

.c.o:
	@echo -e "\tCC $<"
	@$(CC) $(WFLAGS) $(CFLAGS) -c $(LFLAGS) $< -o $@


all : $(OUT)

prepare:
	@mkdir -p $(OUTDIR)

copy_header_files:
	@mkdir -p $(OUTDIR)/include
	@cp src/havarg.h $(OUTDIR)/include/
	@cp src/common.h $(OUTDIR)/include/

$(OUT): prepare $(OBJS) copy_header_files
	@mkdir -p $(OUTDIR)
	@echo -e "\tAR $(OBJS)"
	@ar -rcs $(OUT) $(OBJS)

	@echo ""
	@echo "Output: $(OUT)"
	@echo "Finished."

.PHONY: dbg_clean debug clean gen_cflags

dbg_clean:
	@echo -e "\tRM $(OBJS)"
	@rm -f $(OBJS)

debug: dbg_clean
	@mkdir -p $(DBGOUTDIR)
	@CFLAGS+="-Ddebug -g3" OUTDIR=$(DBGOUTDIR) $(MAKE) all


clean:
	@echo -e "\tRM $(OBJS) $(OUT) build/ debug/"
	@rm -rf $(OBJS) $(OUT) build/ debug/

gen_cflags:
	@echo $(WFLAGS) $(CFLAGS) $(LFLAGS) | sed -z 's/ /\n/g'
	@echo -e "\t|\n\tV"
	@echo "$(CFLAGS_FILE)"
