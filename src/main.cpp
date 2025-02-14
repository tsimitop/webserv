/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daspring <daspring@student.42heilbronn.de  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 12:51:41 by daspring          #+#    #+#             */
/*   Updated: 2025/02/14 12:16:07 by daspring         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include "./sockets.cpp"

int	main(int argc, char** argv)
{
	start_server();
	return EXIT_SUCCESS;
}
