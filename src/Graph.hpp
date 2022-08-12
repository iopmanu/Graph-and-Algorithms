#ifndef SRC_GRAPH_HPP_
#define SRC_GRAPH_HPP_

#include "ArraySequence.hpp"
#include <iomanip>
#include <iostream>

#define INIT_CONST SIZE_MAX_LOCAL - 100

/*typename T is element weight*/
template <typename T>
class graph {
private:
    array_sequence<array_sequence<T> *> *_graph = NULL;
    bool is_directed = 0;

public:
    /*==================================CONSTRUCTORS==================================*/

    explicit graph(const std::size_t &elements_count, const bool &direction) noexcept
        : _graph(new array_sequence<array_sequence<T> *>(elements_count)), is_directed(direction) {
        for (std::size_t i = 0; i < _graph->get_size(); i++) {
            _graph->operator[](i) = new array_sequence<T>(elements_count);
        }
    }

    explicit graph(const std::size_t &elements_count, const bool &direction, const T &init_value) noexcept
        : _graph(new array_sequence<array_sequence<T> *>(elements_count)), is_directed(direction) {
        for (std::size_t i = 0; i < _graph->get_size(); i++) {
            _graph->operator[](i) = new array_sequence<T>(elements_count, init_value);
        }
    }

    ~graph() = default;

    /*==================================OPERATORS==================================*/

    friend std::ostream &operator<<(std::ostream &out, graph<T> *source) noexcept {
        out << "\t";

        for (std::size_t i = 0; i < source->_graph->get_size(); i++) {
            out << i << "\t";
        }

        out << std::endl << std::endl;

        for (std::size_t i = 0; i < source->_graph->get_size(); i++) {
            out << i << source->_graph->operator[](i) << std::endl;
        }

        return out;
    }

    /*==================================LOOKUP==================================*/

    constexpr std::size_t get_elements_quantity() const noexcept { return this->_graph->get_size(); }

    constexpr std::size_t empty() const noexcept { return this->_graph->empty(); }

    /*==================================METHODS==================================*/

    void add_edge(const std::size_t &first_element, const std::size_t &second_element, const T &weight) noexcept {
        this->_graph->operator[](second_element)->operator[](first_element) = std::move(weight);
        if (this->is_directed == false) {
            this->_graph->operator[](first_element)->operator[](second_element) = std::move(weight);
        }
    }

    void delete_edge(const std::size_t &first_element, const std::size_t &second_element) noexcept {
        this->_graph->operator[](second_element)->operator[](first_element) = std::move(SIZE_MAX_LOCAL);
        if (this->is_directed == false) {
            this->_graph->operator[](first_element)->operator[](second_element) = std::move(SIZE_MAX_LOCAL);
        }
    }

    void add_vertex() noexcept {
        this->_graph->append(new array_sequence<T>(this->get_elements_quantity(), SIZE_MAX_LOCAL));

        for (std::size_t i = 0; i < this->get_elements_quantity(); i++) {
            _graph->operator[](i)->append(SIZE_MAX_LOCAL);
        }
    }

    void delete_vertex() noexcept {
        for (std::size_t i = 0; i < this->get_elements_quantity(); i++) {
            this->delete_edge(i, this->get_elements_quantity() - 1);
            _graph->operator[](i)->pop_back();
        }

        _graph->pop_back();
    }

    const T &get_edge_weight(const std::size_t &first_element, const std::size_t &second_element) const noexcept {
        return this->_graph->operator[](first_element)->operator[](second_element);
    }

    array_sequence<std::size_t> *find_shortest_path_djkstra(const std::size_t &started_element) const {
        auto path_sequence = new array_sequence<std::size_t>(_graph->get_size(), SIZE_MAX_LOCAL);
        auto parents_sequence = new array_sequence<std::size_t>(_graph->get_size());
        auto visited_sequence = new array_sequence<bool>(_graph->get_size(), false);

        path_sequence->operator[](started_element) = 0;

        for (std::size_t i = 0; i < this->get_elements_quantity(); i++) {
            std::size_t vertex = INIT_CONST;

            for (std::size_t j = 0; j < this->get_elements_quantity(); j++) {
                if (!visited_sequence->operator[](j) &&
                    (vertex == INIT_CONST || path_sequence->operator[](j) < path_sequence->operator[](vertex))) {
                    vertex = j;
                }
            }

            if (path_sequence->operator[](vertex) == SIZE_MAX) {
                break;
            }

            visited_sequence->operator[](vertex) = true;

            for (std::size_t j = 0; j < this->get_elements_quantity(); j++) {
                if (_graph->operator[](vertex)->operator[](j) == T(0)) {
                    continue;
                }

                std::size_t length = _graph->operator[](vertex)->operator[](j);

                if (path_sequence->operator[](vertex) + length < path_sequence->operator[](j)) {
                    path_sequence->operator[](j) = path_sequence->operator[](vertex) + length;
                    parents_sequence->operator[](j) = vertex;
                }
            }
        }

        return path_sequence;
    }

    graph<std::size_t> *find_all_shortest_path_wallsher() {
        auto path_matrix = new graph<std::size_t>(this->get_elements_quantity(), this->is_directed, SIZE_MAX_LOCAL);

        for (std::size_t k = 0; k < _graph->get_size(); k++) {
            for (std::size_t i = 0; i < _graph->get_size(); i++) {
                for (std::size_t j = 0; j < _graph->get_size(); j++) {
                    if (_graph->operator[](i)->operator[](j) == T(0) || _graph->operator[](i)->operator[](k) == T(0) ||
                        _graph->operator[](k)->operator[](j) == T(0)) {
                        continue;
                    }

                    path_matrix->add_edge(i, j,
                                          std::min(this->get_edge_weight(i, j), this->get_edge_weight(i, k) + this->get_edge_weight(k, j)));
                }
            }
        }

        return path_matrix;
    }

    graph<std::size_t> *get_adjency_matrix() const noexcept {
        auto adjency_matrix = new graph<std::size_t>(this->_graph->get_size(), this->is_directed);

        for (std::size_t i = 0; i < _graph->get_size(); i++) {
            for (std::size_t j = 0; j < _graph->get_size(); j++) {
                if (this->_graph->operator[](i)->operator[](j) != T(0)) {
                    adjency_matrix->add_edge(i, j, true);
                }
            }
        }

        return adjency_matrix;
    }

    array_sequence<std::size_t> *find_minimal_spanning_tree() const noexcept {
        auto spanning_sequence = new array_sequence<T>(_graph->get_size(), SIZE_MAX_LOCAL);   // edges weight
        auto edge_end = new array_sequence<std::size_t>(_graph->get_size(), SIZE_MAX_LOCAL);  // ends of edges
        auto spanning_tree = new array_sequence<std::size_t>();  // sequence for minimal spanning tree edges(to output)
        auto is_used = new array_sequence<bool>(_graph->get_size(), false);
        spanning_sequence->operator[](0) = T(0);

        for (std::size_t i = 0; i < this->get_elements_quantity(); i++) {
            std::size_t vertex = INIT_CONST;

            for (std::size_t j = 0; j < this->get_elements_quantity(); j++) {
                if (!is_used->operator[](j) &&
                    (vertex == INIT_CONST || spanning_sequence->operator[](j) < spanning_sequence->operator[](vertex))) {
                    vertex = j;
                }
            }

            if (spanning_sequence->operator[](vertex) == SIZE_MAX_LOCAL) {
                return new array_sequence<std::size_t>(_graph->get_size(), SIZE_MAX_LOCAL);
            }

            is_used->operator[](vertex) = true;

            if (edge_end->operator[](vertex) != SIZE_MAX_LOCAL) {
                spanning_tree->append(vertex);
                spanning_tree->append(edge_end->operator[](vertex));
            }

            for (std::size_t j = 0; j < this->get_elements_quantity(); j++) {
                if (this->get_edge_weight(vertex, j) < spanning_sequence->operator[](j)) {
                    spanning_sequence->operator[](j) = this->get_edge_weight(vertex, j);
                    edge_end->operator[](j) = vertex;
                }
            }
        }

        return spanning_tree;
    }
};

#endif  // SRC_GRAPH_HPP_
