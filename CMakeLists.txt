
cmake_minimum_required(VERSION 3.3)
project(game_text C)

include(CTest)
enable_testing()
link_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_C_FLAGS "-std=c99 -g -Wall --coverage")

include(sdl2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

#-------------------------------------------------------------------------------------------

add_library(queue STATIC queue.c)
add_library(game game.c game_aux.c game_ext.c queue game_tools.c)
add_executable(save test_save.c)
add_executable(game_text game_text.c)
add_executable(game_test_mwache game_test_mwache.c)
add_executable(game_test_adejesus game_test_adejesus.c)
add_executable(game_test_hyoussouf game_test_hyoussouf.c)
add_executable(test_queue test_queue.c) # Test par Aurelien Esnard
add_executable(game_random game_random.c)
add_executable(game_solve game_solve.c)
target_link_libraries(game_text game)
target_link_libraries(game_test_adejesus game)
target_link_libraries(game_test_mwache game)
target_link_libraries(game_test_hyoussouf game)
target_link_libraries(test_queue queue)
target_link_libraries(game_random game)
target_link_libraries(save game)
target_link_libraries(game_solve game)

#-------------------------------------------------------------------------------------------

include_directories(${SDL2_ALL_INC})
add_executable(game_sdl main.c game_sdl.c)
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} m game)

file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#-------------------------------------------------------------------------------------------
add_test(test_mwache_dummy ./game_test_mwache dummy)
add_test(test_mwache_game_default ./game_test_mwache game_default)
add_test(test_mwache_game_default_solution ./game_test_mwache game_default_solution)
add_test(test_mwache_game_get_ajacent_square ./game_test_mwache game_get_ajacent_square)
add_test(test_mwache_game_has_half_edge ./game_test_mwache game_has_half_edge)
add_test(test_mwache_game_check_edge ./game_test_mwache game_check_edge)
add_test(test_mwache_game_is_well_paired ./game_test_mwache game_is_well_paired)
add_test(test_mwache_game_is_connected ./game_test_mwache game_is_connected)
add_test(test_mwache_game_new_ext ./game_test_mwache game_new_ext)
add_test(test_mwache_game_save ./game_test_mwache game_save)
#---------------------------------------------------------------------------------------------------
add_test(test_adejesus_dummy ./game_test_adejesus dummy)
add_test(test_adejesus_game_get_piece_shape ./game_test_adejesus game_get_piece_shape)
add_test(test_adejesus_game_get_piece_orientation ./game_test_adejesus game_get_piece_orientation)
add_test(test_adejesus_game_play_move ./game_test_adejesus game_play_move)
add_test(test_adejesus_game_won ./game_test_adejesus game_won)
add_test(test_adejesus_game_reset_orientation ./game_test_adejesus game_reset_orientation)
add_test(test_adejesus_game_shuffle_orientation ./game_test_adejesus game_shuffle_orientation)
add_test(test_adejesus_game_print ./game_test_adejesus game_print)
add_test(test_adejesus_game_redo ./game_test_adejesus game_redo)
add_test(test_adejesus_game_load ./game_test_adejesus game_load)
#---------------------------------------------------------------------------------------------------
add_test(test_hyoussouf_dummy ./game_test_hyoussouf dummy)
add_test(test_hyoussouf_game_new_empty ./game_test_hyoussouf game_new_empty)
add_test(test_hyoussouf_game_new ./game_test_hyoussouf game_new)
add_test(test_hyoussouf_game_equal ./game_test_hyoussouf game_equal)
add_test(test_hyoussouf_game_copy ./game_test_hyoussouf game_copy)
add_test(test_hyoussouf_game_delete ./game_test_hyoussouf game_delete)
add_test(test_hyoussouf_game_set_piece_shape ./game_test_hyoussouf game_set_piece_shape)
add_test(test_hyoussouf_game_set_piece_orientation ./game_test_hyoussouf game_set_piece_orientation)
add_test(test_hyoussouf_game_new_empty_ext ./game_test_hyoussouf game_new_empty_ext)
add_test(test_hyoussouf_game_nb_rows ./game_test_hyoussouf game_nb_rows)
add_test(test_hyoussouf_game_nb_cols ./game_test_hyoussouf game_nb_cols)
add_test(test_hyoussouf_game_undo ./game_test_hyoussouf game_undo)
add_test(test_hyoussouf_game_is_wrapping ./game_test_hyoussouf game_is_wrapping)
add_test(test_hyoussouf_game_load ./game_test_hyoussouf game_load)

# Test par Aurelien Esnard de la structure de donnée "queue" faites par Aurelien Esnard

add_test(test_queue_new_free ./test_queue new_free)
add_test(test_queue_push_head ./test_queue push_head)
add_test(test_queue_pop_head ./test_queue pop_head)
add_test(test_queue_push_tail ./test_queue push_tail)
add_test(test_queue_pop_tail ./test_queue pop_tail)
add_test(test_queue_length ./test_queue length)
add_test(test_queue_empty ./test_queue empty)
add_test(test_queue_clear ./test_queue clear)