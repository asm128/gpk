IncDir			:= ../../zlib 
Platform		:= Linux.x64
ProjectName		:= llc
Configuration	:= Release

C_DEFS			:= -DNOMINMAX -DGPK_XCB
OPT				:= 3

ifeq ($(DEBUG), 1)
	Configuration	= Debug
	OPT				= 0
	C_DEFS			+= -DDEBUG
endif

OutDir	= ../../$(Platform).$(Configuration)
IntDir	= ../../obj/$(Platform).$(Configuration)/$(ProjectName)

OBJDIRS	:= $(patsubst %, $(IntDir), $(ProjectName))
INCDIR	:= $(patsubst %,-I%,$(IncDir))

CFLAGS	:= -std=c++23 -Wall -Werror -O$(OPT) $(INCDIR) 
CC		:= g++

Src	:= gpk_adam7.cpp \
	   gpk_aes.cpp \
	   gpk_append_css.cpp \
	   gpk_append_html.cpp \
	   gpk_append_js.cpp \
	   gpk_append_json.cpp \
	   gpk_append_tcpip.cpp \
	   gpk_append_xml.cpp \
	   gpk_args.cpp \
	   gpk_array.cpp \
	   gpk_ascii_target.cpp \
	   gpk_base64.cpp \
	   gpk_block.cpp \
	   gpk_broker.cpp \
	   gpk_cgi.cpp \
	   gpk_cgi_main.cpp \
	   gpk_cgi_module.cpp \
	   gpk_cgi_runtime.cpp \
	   gpk_chrono.cpp \
	   gpk_circuit.cpp \
	   gpk_client_app.cpp \
	   gpk_component_scene.cpp \
	   gpk_component_scene_draw.cpp \
	   gpk_connection.cpp \
	   gpk_connection_ip.cpp \
	   gpk_control_grid.cpp \
	   gpk_crc.cpp \
	   gpk_d3d.cpp \
	   gpk_datatype.cpp \
	   gpk_debug.cpp \
	   gpk_deflate.cpp \
	   gpk_dialog.cpp \
	   gpk_encoding.cpp \
	   gpk_expression.cpp \
	   gpk_file.cpp \
	   gpk_file_pack.cpp \
	   gpk_font.cpp \
	   gpk_framework.cpp \
	   gpk_framework_module.cpp \
	   gpk_geometry.cpp \
	   gpk_geometry_draw.cpp \
	   gpk_geometry_lh.cpp \
	   gpk_gltf.cpp \
	   gpk_gui.cpp \
	   gpk_gui_control.cpp \
	   gpk_gui_control_list.cpp \
	   gpk_gui_draw.cpp \
	   gpk_gui_inputbox.cpp \
	   gpk_gui_system.cpp \
	   gpk_gui_theme.cpp \
	   gpk_gui_viewport.cpp \
	   gpk_http_client.cpp \
	   gpk_https_client.cpp \
	   gpk_i2c.cpp \
	   gpk_index.cpp \
	   gpk_index_catalog.cpp \
	   gpk_index_path.cpp \
	   gpk_json.cpp \
	   gpk_json_expression.cpp \
	   gpk_keyval_view.cpp \
	   gpk_label.cpp \
	   gpk_log.cpp \
	   gpk_mapblock.cpp \
	   gpk_noise.cpp \
	   gpk_parse.cpp \
	   gpk_particle.cpp \
	   gpk_path.cpp \
	   gpk_platform_error.cpp \
	   gpk_png.cpp \
	   gpk_png_write.cpp \
	   gpk_process.cpp \
	   gpk_process_linux.cpp \
	   gpk_process_win32.cpp \
	   gpk_raster_lh.cpp \
	   gpk_rsa.cpp \
	   gpk_scene.cpp \
	   gpk_server_app.cpp \
	   gpk_socket_client.cpp \
	   gpk_socket_server.cpp \
	   gpk_spi.cpp \
	   gpk_stdstring.cpp \
	   gpk_stl.cpp \
	   gpk_string.cpp \
	   gpk_string_compose.cpp \
	   gpk_string_helper.cpp \
	   gpk_string_time.cpp \
	   gpk_tcpip.cpp \
	   gpk_test_core.cpp \
	   gpk_timer.cpp \
	   gpk_view.cpp \
	   gpk_virtual_keyboard.cpp \
	   gpk_voxel_geometry.cpp \
	   gpk_window.cpp \
	   gpk_windows.cpp \
	   gpk_xml_reader.cpp \
	   llc.cpp

_OBJ	:= $(patsubst %.cpp,%.o,$(Src))
OBJ		:= $(patsubst %,$(IntDir)/%,$(_OBJ))

$(IntDir)/%.o: %.cpp
	$(CC) $(CFLAGS) $(C_DEFS) -c -o $@ $<

build: $(OBJDIRS) $(OBJ)
	mkdir -p $(OutDir)
	ar rcs $(OutDir)/$(ProjectName).a $(OBJ)

$(OBJDIRS):
	mkdir -p $@ 

.PHONY: clean

clean:
	rm -rf $(IntDir)
