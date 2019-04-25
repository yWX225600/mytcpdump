INC_DIR=-I$(PWD)/include \
		-I$(PWD)/libpcap/include \
		-I$(PWD)/wlog/include \
		-I$(PWD)/inipares
		
SRC_DIR=$(PWD)/source
CLOG_SRC_DIR=$(PWD)/wlog/source
INI_DIR=$(PWD)/inipares

LIB_CON=-lm -lpthread \
		-L$(PWD)/libpcap/lib -Wl,-rpath=$(PWD)/libpcap/lib -lpcap

CXX=g++ -O2 -rdynamic -g

DEFINE=-D_OS_TYPE=32 -D_LOG -D_DEBUG

OBJS_LAST=./bin/mytcpdump

OBJS_MIDD=$(SRC_DIR)/main.o $(SRC_DIR)/MyPcapDump.o $(SRC_DIR)/ProtocolAnalysis.o $(SRC_DIR)/IpProtoAnal.o $(SRC_DIR)/TcpProtoAnal.o $(SRC_DIR)/UdpProtoAnal.o \
			$(SRC_DIR)/ACQueue.o $(SRC_DIR)/DealProtPkg.o $(SRC_DIR)/OutProtPkg.o $(SRC_DIR)/TaskQueue.o $(SRC_DIR)/SFTool.o \
			$(CLOG_SRC_DIR)/CLog.o \
			$(INI_DIR)/iniconfile.o
			


$(OBJS_LAST):$(OBJS_MIDD)
	$(CXX) $^ -o $@ $(LIB_CON)

-include $(OBJS_MIDD:.o=.d)

%.o:%.cpp
	$(CXX) -c $*.cpp -o $*.o $(INC_DIR) $(DEFINE) $(LIB_CON)
	$(CXX) -MM -cpp $*.cpp $(INC_DIR) $(DEFINE) $(LIB_CON) > $*.d.$$$$; \
	sed 's,$(@F),$*.o $*.d ,g' < $*.d.$$$$ > $*.d; \
	sed 's,\\,,g' < $*.d.$$$$ | fmt -1 | \
	sed -e '1d' -e 's,[ ],,g' -e 's,$$,:,g' >> $*.d; \
	$-$(RM) $*.d.$$$$
	
.PHONY:clean
clean:
	-$(RM) $(SRC_DIR)/*.o $(SRC_DIR)/*.d*
	-$(RM) $(CLOG_SRC_DIR)/*.o $(CLOG_SRC_DIR)/*.d*
	-$(RM) $(INI_DIR)/*.o $(INI_DIR)/*.d*
	
.PHONY:cleanall
cleanall:
	-$(RM) $(SRC_DIR)/*.o $(SRC_DIR)/*.d*
	-$(RM) $(CLOG_SRC_DIR)/*.o $(CLOG_SRC_DIR)/*.d*
	-$(RM) $(INI_DIR)/*.o $(INI_DIR)/*.d*
	-$(RM) $(OBJS_LAST)
	
